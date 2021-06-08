print("Comparing logs")

myLog = open("Logs/emul.log").read().split("\n")
sourceLog = open("Logs/zelda.log").read().split("\n")

numberOfLines = min(len(myLog),len(sourceLog))

#numberOfLines = min(numberOfLines,20) #limit to 20 lines

startLine = 0
myOffset = 0

#Syncing the logs together
#[myLogLine,sourceLogLine]
syncLogs = [[1697,1671],[1744,1716],[1857,1827],[1877,1849]]
syncLogs = []
for syncLine in syncLogs:
    startLine = syncLine[1]
    myOffset = syncLine[0]-syncLine[1]

def checkLoopSync(line):
    elements = line.split()
    return elements[1] == "cmp" and elements[2] == "$2140"

syncingBNEmyAdvance = False
syncingBNEsourceAdvance = False
print("Comparing",numberOfLines,"lines")
#for i in range(startLine,numberOfLines):
i = startLine
while i != numberOfLines-1:
    if syncingBNEmyAdvance:
        myOffset -= 2
    if syncingBNEsourceAdvance:
        myOffset += 2
    myLine = " ".join(myLog[i+myOffset].split())
    sourceLine = sourceLog[i]

    import re
    sourceLine = re.sub(r"\[[0-9a-f]{6}\] A","A",sourceLine)
    sourceLine = sourceLine[:-16]

    sourceLine = " ".join(sourceLine.split())

    if myLine != sourceLine:
        if checkLoopSync(sourceLog[i+1]):#if next line is a cmp again, or different test: compare line i-1 with i to see which log deviates
            syncingBNEmyAdvance = True
        elif checkLoopSync(myLog[i+myOffset+1]):
            syncingBNEsourceAdvance = True
        else:
            print("wtf")
            #print(myLine)
            #print(sourceLine)
            #print("SourceLigne:",i+1)

            print("Erreur Capitaine !")
            print("Ma ligne : "+myLine)
            print("La Source:",sourceLine)
            print("SourceLigne:",i+1)
            print("MyLigne    :",i+myOffset+1)
            break
    else:
        syncingBNEsourceAdvance = False
        syncingBNEmyAdvance = False
    if not syncingBNEsourceAdvance: #block source idx until my log catches up
         i += 1
    