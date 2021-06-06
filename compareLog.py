print("Comparing logs")

myLog = open("emul.log").readlines()
sourceLog = open("Games/zelda.log").readlines()

numberOfLines = min(len(myLog),len(sourceLog))

print("Comparing",numberOfLines,"lines")
for line in myLog:
    print(line.strip())