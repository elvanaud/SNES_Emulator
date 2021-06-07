print("Comparing logs")

myLog = open("Logs/emul.log").read().split("\n")
sourceLog = open("Logs/zelda.log").read().split("\n")

numberOfLines = min(len(myLog),len(sourceLog))

#numberOfLines = min(numberOfLines,20) #limit to 20 lines

startLine = 0

#Syncing the logs together
#[myLogLine,sourceLogLine]
syncLogs = [[1697,1671],[1744,1716],[1857,1827],[1877,1849]]
for syncLine in syncLogs:
    startLine = syncLine[1]
    myOffset = syncLine[0]-syncLine[1]

print("Comparing",numberOfLines,"lines")
for i in range(startLine,numberOfLines):
    myLine = " ".join(myLog[i+myOffset].split())
    sourceLine = sourceLog[i]

    import re
    sourceLine = re.sub(r"\[[0-9a-f]{6}\] A","A",sourceLine)
    #sourceLine = sourceLine.split("V")[0]
    sourceLine = sourceLine[:-16]

    sourceLine = " ".join(sourceLine.split())

    if myLine != sourceLine:
        print("Erreur Capitaine !")
        print("Ma ligne : "+myLine)
        print("La Source:",sourceLine)
        print("SourceLigne:",i+1)
        print("MyLigne    :",i+myOffset+1)
        break
    