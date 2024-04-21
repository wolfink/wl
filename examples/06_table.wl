load std.*; // "*" loads all libraries in std

// Program calls print_number on a line of user input
print_number(?#read_line());
println#'\b' // remove extra space character and ends line

// table macro creates a number of arrays that map onto given indices
// structure is table#name(arg: type, ...) { num => (arg_value, ...), ... }
table#ones_tbl(string: string) {
  0 => ("zero"),
  1 => ("one"),
  2 => ("two"),
  3 => ("three"),
  4 => ("four"),
  5 => ("five"),
  6 => ("six"),
  7 => ("seven"),
  8 => ("eight"),
  9 => ("nine")
}

table#teens_tbl(string: string) {
  0 => ("ten"),
  1 => ("eleven"),
  2 => ("twelve"),
  3 => ("thirteen"),
  4 => ("fourteen"),
  5 => ("fifteen"),
  6 => ("sixteen"),
  7 => ("seventeen"),
  8 => ("eighteen"),
  9 => ("nineteen"),
}

table#tens_tbl(string: string) {
  0 => ("")
  1 => ("ten"),
  2 => ("twenty"),
  3 => ("thirty"),
  4 => ("fourty"),
  5 => ("fifty"),
  6 => ("sixty"),
  7 => ("seventy"),
  8 => ("eighty"),
  9 => ("ninety"),
}

// table below is equivalent to statement:
// order_tabl: (next_order: 32[11] = [ **values** ], string: string[11] = [ **values** ]);
table#order_tbl(next_order: 32, string: string) {
  0 => (0, ""),
  1 => (0, "ones"),
  2 => (1, "tens"),
  3 => (3, "hundred"),
  4 => (4, "thousand"),
  5 => (4, "thousand"),
  6 => (4, "thousand"),
  7 => (7, "million"),
  8 => (7, "million"),
  9 => (7, "million"),
  10 => (10, "billion")
}

// Prints numbers in English
// Example: print_number(1345437) =>
//   "one million three hundred fourty five thousand four hundred thirty seven "
print_number: (n: const string) => ()
{
  // <# and ># macros allow you to chain comparisons together
  // below is equivalent to writing (0 < n.len && n.len < 11)
  if !<#(0, n.len, 11) { return; } 
  order := n.len;
  if order > 2 {
    // String slice returns a tuple: the string before the slice, and after
    (ord_prefix: string, n) = n->slice(str.len - order_tbl.next_order[order] + 1);
    print_number(ord_prefix);
    print# "$(order_tbl.string[order]) ";
    print_number(n);
    return;
  }
  if order == 2 {
    if n[0] == '1' { 
      teen_idx := n[1];
      print# $(teens_tbl.string[teen_idx]);
    } else {
      tens_idx := n[0];
      print# "$(tens_tbl.string[tens_idx]) ";
      print_number(n->slice(1).1) // ".1" takes the first argument of a tuple
    }
  }
  if order == 1 {
    ones_idx := n[0];
    print# "$(ones_tbl.string[ones_idx]) ";
  }
}
