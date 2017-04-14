
//
This is an interpreter for the esoteric programming language brainfuck
\\

data: IntArray: 40000
offset: 0
error: fls
inText: ""

j: 0 | j<data.len | j: j+1 @
	data.set: j, 0

code: "enter your brainfuck code: ".input
#code: "++++++++++[>+++++++>++++++++++>+++>+<<<<-]>++.>+.+++++++..+++.>++.<<+++++++++++++++.>.+++.------.--------.>+.>."

processSection: code, 0, tru

error ?
(
	print: "program aborted due to error"
	print: "position at exit:"
	print: offset
)

# runs a loop of bf code, returns when done 
processSection :: {src: String, start: Int, isTopLevel: Bool}: (
	
	i: (in.start)
	quit: fls
	
	!quit && !error @ (
		c: in.src.sub: i, i+1
		
		c = "<" ? (
			offset: offset-1
			offset < 0 ? (
				print: "you went too far left"
				error: tru
			)
		)
		| c = ">" ? (
			offset: offset+1
			offset >= data.len ? (
				print: "you went too far right"
				error: tru
			)
		)
		| c = "+" ? (
			data.set: offset, (data.get: offset)+1
		)
		| c = "-" ? (
			data.set: offset, (data.get: offset)-1
		)
		| c = "." ? (
			printc: data.get: offset
		)
		| c = "," ? (
			data.set: offset, inputc
		)
		| c = "[" ? (
			processSection: in.src, i+1, fls
			i: skipBrace: in.src, i
		)
		| c = "]" ? (
			in.isTopLevel ? (
				print: "']' without matching '['"
				error: tru
			)|(
				(Bool: data.get: offset) ? (
					i: in.start-1
				)|(
					quit: tru
				)
			)
		)
		
		i: i+1
		
		i >= in.src.len ? (
			in.isTopLevel ? (
				quit: tru
			)|(
				print: "'[' without matching ']'"
				error: tru
			)
		)
	)
)

skipBrace :: {src: String, i: Int} -> {Int}: (
	
	i: in.i
	
	(in.src.sub: i, i+1) != "[" ? (
		print: "skipBrace called on character other then '['"
		error: tru
	)|(
	
	i: i+1
	
	!error && (in.src.sub: i, i+1) != "]" @ (
		i: i+1
		i >= in.src.len ? (
			print: "'[' without matching ']'"
			error: tru
		)
	)
	
	)
											
	i
)

#>++++++++[-<+++++++++>]<.>>+>-[+]++>++>+++[>[->+++<<+++>]<<]>-----.>->+++..+++.>-.<<+[>[+>+]>>]<--------------.>>.+++.------.--------.>+.>+.

