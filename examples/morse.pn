# this program converts text into morse code

# this constant string will act as the data source for the conversion
key :: "a.- b-... c-.-. d-.. e. f..-. g--. h.... i.. j.--- k-.- l.-.. m-- n-. o--- p.--. q--.- r.-. s... t- u..- v...- w.-- x-..- y-.-- z--.. 1.---- 2..--- 3...-- 4....- 5..... 6-.... 7--... 8---.. 9----. 0----- ..-.-.- ?..--.. "

# loop until the user types "quit"

quit: fls

!quit @
(
	# get input
	inStr: "enter some text or type quit: ".input
	
	inStr = "quit" ?
		quit: tru
	|
		print: stringToMorse: inStr
)

print

stringToMorse :: {String} -> {String}:
(
	out: ""
	i: 0 | i<in.len | i: i+1 @
	(
		letter: in.sub: i, i+1
		
		letter = " " ?
			out: out+"    "
		|
			out: out+(letterToMorse: letter)+" "
	)
	
	out
)

letterToMorse :: {String} -> {String}:
(	
	out: ""
	
	found: fls
	
	i: 0 | i<key.len && !found | i: i+1 @
	(
		(key.sub: i, i+1) = in ?
		(
			j: i+1
			 
			!((key.sub: j, j+1) = " ") @
				j: j+1
			
			out: key.sub: i+1, j
			
			found: tru
		)
	)
	
	!found ?
		out: "[" + in + "]"
	
	out
)
