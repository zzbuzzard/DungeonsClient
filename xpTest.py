import turtle

def exp(n):
    if n<=10:
        return 1000*n
    if n<=20:
        return 2000*n-10000
    if n<=30:
        return 4000*n-50000
    if n<=40:
        return 7500*n-155000
    if n<=50:
        return 15000*n-455000
    if n<=60:
        return 30000*n-1205000
    if n<=70:
        return 50000*n-2405000
    if n<=80:
        return 100000*n-5905000
    if n<=90:
        return 150000*n-9905000
    if n<=98:
        return 300000*n-23405000
    return 6500000

# Helps generating exp function
def num(mul, firstLev):
	prev = exp(firstLev-1)
	print("Level",firstLev-1,"=",prev)
	goal = prev + mul
	k = mul*firstLev - goal
	print("Level",firstLev,"=",mul*firstLev-k)
	return k

# Returns the level you'd be at with XP=xp
def getLevel(xp):
    n = 1
    while True:
        t = exp(n)
        if t <= xp:
            xp -= t
            n += 1
        else:
            return n

# Returns cumulative XP to reach level n
def getXP(n):
    t = 0
    for i in range(1, n):
        t += exp(i)
    return t

inp = input("Type D to draw XP: ")
if inp.upper() == "D":
    X = 500
    Y = 325

    maxHeight = 650
    div = exp(99)/maxHeight

    # exp(99)/div = maxHeight

    t = turtle.Turtle()
    t.speed(0)

    for i in range(1, 100):
        x = i*10 - X
        y = exp(i)/div - Y
        t.up()
        t.goto(x, -Y)
        t.down()
        t.goto(x, y)
        if i%10==1:
            t.up()
            t.goto(x-25,y)
            t.down()
            t.write("Lev "+str(i))

print()
inp = input("How many hours should it take to beat the game?: ")
h = float(inp)

totXP = getXP(100)

# totXp in h Hours
# totXp/h in 1 h

perHour = totXP / h
# XP/h

print()
print("Good players should average",int(round(perHour, 0)),"XP per hour")

print("If a dungeon takes 15 min,",int(round(perHour/4,0)),"XP per dungeon")
print("And users would have to do",int(round(h*4,0)),"dungeons to hit lv100")

print()
while True:
    n = int(input("Type a level: "))
    print("Level",n)
    print("Total XP:",getXP(n))
    print("To reach next level:",exp(n))

    print("Estimated time:", round(getXP(n)/perHour, 1), "hours")
    print("Estimated time stuck on this level:", round(exp(n)/perHour, 1), "hours")
    print()

