# FizzBuzz

# call the function defined below
fizzBuzz: 1, 20

# define the FizzBuzz function
fizzBuzz :: {start: Int, end: Int}: (
	
	# loop i from start to end
	i: in.start | i <= in.end | i: i+1 @ (
		
		# use conditionals to print the right thing
		
		i % 3 = 0 && i % 5 = 0 ?
			print: "FizzBuzz"
		|
		i % 3 = 0 ?
			print: "Fizz"
		|
		i % 5 = 0 ?
			print: "Buzz"
		|
			print: i
	)
)
