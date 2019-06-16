
# dec2bin

Decimal to binary command-line conversion utility.

# Usage

Internally, numbers are represented as `mpz_t`, which is GNU GMP's arbitrary-
precision integral data type. Numbers of any size may therefore be represented,
so, theoretically speaking, the limit is your computer's memory, and in more
practical terms, the limit is how long you're willing to sit in front of your
computer waiting for the result.

### Examples

For simple conversions you can simply pass in your desired inputs.

```bash
$ ./dec2bin 845777
11001110011111010001
```

Should you wish to configure the output to look a little nicer, or at least
make it easier to read, the current options available are `--pretty-print` and
`--group-size`. The former pads the output to be even groups of `n`, where `n`
is the group size, and the latter actually specifies a custom group size.

If you specify `--pretty-print` without setting a group size, the program will use
the default, which is 4. If you specify a group size without actually requesting
for the program to pretty-print your output, the program will disregard your
request and print the output as if you had specified no options at all. I might
change this in the future to allow the `--group-size` option to imply
`--pretty-print`, but at the moment the program does exactly what you <em>explicitly</em>
tell it to; nothing more, nothing less. I mean this literally, as specifying a
group size without specifying the `--pretty-print` option <strong>will</strong> set
the group size, but <strong>will not</strong> route the output through the 
subroutine that would use it.

Here is an example of a custom group size setting with a large input integer.

```bash
$ ./dec2bin --pretty-print --group-size 8 683543568763521656876541231
00000010 00110101 01101001 11111011 00011100 10110010 01000110 00110001 11110110 10110001 10100101 00101111
```

# Building

The program uses the [GNU Multiple Precision library](https://gmplib.org/) to 
handle inputs of any size, so this is the only real requirement. Besides that,
the application is written in C17-conforming [standard C](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2346.pdf),
so as long as you have a conforming C compiler you should have no problem 
building the application, aforementioned pre-requisites notwithstanding.

The makefile is currently set to my default options, so you cannot currently
override any settings via the command-line. I apologize for this, but you may
modify the makefile to allow for this, or you may simply edit it as you see fit.

Pull requests are welcome, and feel free to drop me a line if you have any 
problems, questions, concerns, bug reports, etc.

