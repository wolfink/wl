// Generates a table listing fahrenheight and celsius values
load std.io;

// Range type syntax: 1,2..n OR 1,+1..n
f := 0,20..300; // Range types act as a for loop
c := (5 / 9) * f - 32; // You can use a range of values the same as a single value

// Ranges execute a full statement before moving onto the next value
// The following line prints out:
// 0        -17
// 20       -6
// ...
// 300      148
println# $f'\t'$c'\t';
