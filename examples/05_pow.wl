// From chapter 1.7 in the C Programming Langauge
load std.io;

// Function declaration
// Format: name: (arg: type, ...) => (return: type) = { **code** }
pow: (m: 32, n: 32) => (out: 32) = {
  out = 1;
  while m -= 1 {
    out *= n;
  }
}

println# pow(3, 4) = $(pow(3, 4)); // returns "pow(3, 4) = 81"
