README addition

Halt test - Ensures that the program halts immediately on the halt command, and
	prints "Bad!\n" if the program proceeds past the halt.

Goto test - Tests to make sure that the Load Program command changes the
        program counter appropriately.  It loads location L into a register,
        and goes to that location in the code which prints "GOTO passed.\n"
        and halts.

Conditional zero test - Tests to make sure that the UM reacts correctly to a
        register that has been loaded with zero.  The code says that if the
        register is nonzero, move to a location which fails.  If not, print
        the success message.

Conditional nonzero test - Same as above, but flipped.  The register is loaded
        with a nonzero value, and it branches accordingly.

Read/Put Char - Reads a single character from input, and outputs it unchanged.

Print Digit - The instructions load the value 5 into a register, add 48 to it,
        and then print the result.  This prints the character '5', because the
        ASCII value of '0' is 48.

NAND print digit - A value is received from input, then the last four bits
        are extracted with a mask of 15 (all zeros with four ones at the end).
        This is obtained using NAND(NAND(input, mask), NAND(input, mask)).
        Then this value is added to 48, and the result is printed.

Run time test - A file with 500000 addition instructions and then a halt.  It
        is used as a judge of our running time.  It completes in 0.075 seconds

Map/Unmap test - Maps a segments and unmaps a segment, then prints out the 
        segment ID that was just mapped.  A properly working implementation
        should print out that segment 1 was just mapped and unmapped.

Add test - Tests the functionality of the add command.

Mult test - Tests the functionality of the multiply command.

Division test - Tests the functionality of the division command.

Cipher test - Takes a string as input, and prints out all the letters shifted
        forwards by one.  A simple Caesar cipher.
