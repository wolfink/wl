// Repeats user input
load std.io;

c_option = read(); // "read" reads a byte and returns an option type
// while syntax: while **expression** { **code** }
while c_option != None {
  c := ?#c_option; // ?# macro calls unwrap function for option types
  write_byte(c);
  c_option = read()
}
