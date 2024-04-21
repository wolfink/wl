// From chapter 1.6 in the C Programming Language
load std.io;

c: 32;
i: 32 = 0;
nwhite: 32 = 0;
nother: 32 = 0;
ndigit: 32[10]; // "type[n]" denotes an array of size n
ndigit[0,1..10] = 0;

c_option := read();
while c_option != None {
  c := ?#c_option;
  if c >= '0' && c <= '9' {
    ndigit[c - '0'] += 1;
  } else if c == ' ' || c == '\n' || c == '\t' {
    nwhite += 1;
  } else {
    nother += 1;
  }
}

print# digits =;

digit = ndigit[0,1..10];
// Statements that use ranges repeat over the entire range.
// The following statment prints all 10 elements of ndigit
// followed by a space.
print# "$digit ";

println# , white space = $nwhite, other = $nother;
