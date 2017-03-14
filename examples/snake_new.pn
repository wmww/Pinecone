# this is an updated version of the original Pinecone snake game
# use WASD to move

w :: 16
h :: 8

meChar :: 79
trailChar :: 43

# init

locX: w/2
locY: h/2

quit: fls

ary: IntArray: w*h

clearBoard

# game loop

!quit @
(
	# update
	
	locX<0? locX: 0
	locX>=w? locX: w-1
	locY<0? locY: 0
	locY>=h? locY: h-1
	
	(ary.get: locY*w+locX)=trailChar ?
	(
		quit: tru
	)|(
		drawBoard
		
		ary.set: locY*w+locX, trailChar
		
		# get input
		
		valid: fls
		
		!valid @
		(
			dir: "use WASD to move, then press return: ".input
			valid: tru
			
			dir="s" ?
				locY: locY+1
			| dir="d" ?
				locX: locX+1
			| dir="w" ?
				locY: locY-1
			| dir="a" ?
				locX: locX-1
			|
				valid: fls
		)
	)
)

clearBoard :: {}:
(
	y: 0 | y<h | y: y+1 @
	(
		x: 0 | x<w | x: x+1 @
		(
			ary.set: y*w+x, 32
		)
	)
)

drawBoard :: {}:
(	
	i: 0 | i<12 | i: i+1 @
		print
	
	line: " "
	
	x: 0 | x<w | x: x+1 @
		line: line+"-"
	
	print: line
	line: ""
	
	y: 0 |y<h |y: y+1 @
	(
		line: line+"|"
		
		x: 0 | x<w | x: x+1 @
		(
			x=locX && y=locY ?
				line: line+(meChar.ascii)
			|
				line: line+(ary.get: y*w+x).ascii
		)
		
		line: line+"|"
		print: line
		line: ""
	)
	
	line: " "
	
	x: 0 | x<w | x: x+1 @
		line: line+"-"
	
	print: line
)

print
printc: 88
printc: 32
printc: 88
print
printc: 45
printc: 45
printc: 45
print
