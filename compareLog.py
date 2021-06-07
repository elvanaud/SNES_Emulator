print("Comparing logs")

myLog = open("emul.log").read().split("\n")
sourceLog = open("Games/zelda.log").read().split("\n")

numberOfLines = min(len(myLog),len(sourceLog))

#numberOfLines = min(numberOfLines,20) #limit to 20 lines

print("Comparing",numberOfLines,"lines")
for i in range(numberOfLines):
    myLine = " ".join(myLog[i].split())
    sourceLine = sourceLog[i]
    """
    if "] A" in sourceLine:
        print("hello")
        print(sourceLine)
        sourceLine = sourceLine.split("[")[0]+sourceLine.split("] A")[1]
    """
    import re
    sourceLine = re.sub(r"\[[0-9a-f]{6}\] A","A",sourceLine)
    sourceLine = sourceLine.split("V")[0]

    sourceLine = " ".join(sourceLine.split())

    #print("cmp:")
    #print(sourceLine)
    #print(myLine)
    if myLine != sourceLine:
        print("Erreur Capitaine !")
        print("Ma ligne : "+myLine)
        print("La Source:",sourceLine)
        print("Ligne:",i)
        break
    