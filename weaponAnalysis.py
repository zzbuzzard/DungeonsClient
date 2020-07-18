weapons = [("Wooden bow", 35, 0.3, 1),
           ("Bone bow", 45, 0.25, 1),
           ("Earth staff", 60, 0.33, 2),
           ("Fire staff", 35, 0.25, 2),
           ("Sandstone sword", 80, 0.2, 1),
           ("Cactus sword", 70, 0.12, 1),
           ("Scorpion dagger", 60, 0.25, 1),
           ("Elven blade", 50, 0.08, 1),
           ("Elven bow", 80, 0.45, 1),
           ("Spider fang dagger", 65, 0.17, 2)]


def damageOf(dmg, df):
    return max(dmg//10, dmg-df)

def weaponDamage(tup, df):
    return damageOf(tup[1], df)/tup[2]

def strround(fl, dp):
    x = str(fl)
    if "." not in x or len(x.split(".")[1]) < dp:
        return x
    p = x.split(".")
    return p[0] + "." + p[1][:dp]

def printAll(df):
    for tup in sorted(weapons, key=lambda tup:-weaponDamage(tup, df)):
        print(tup[0]+":",strround(weaponDamage(tup, df), 1),"*",tup[3])


while True:
    df = int(input("How much defence? "))
    print()
    printAll(df)
    print("\n"*2)
    
