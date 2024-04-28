load std.io

r1 := 0..1...200;
r2 := 0..(+1)...150; // Ranges with parenthesis allows you to specify an operation
// The two statments below are equivalent
// When two ranges in the same statment are unequal,
// creates a nested for loop over both ranges
r3 := range_1 * range_2;
r4 : 32[200][150];
i := 0; while i < 200 {
  j := 0; while j < 150 {
    range_4[i][j] = i * j;
  }
}

linked_list: (start: &ll_node, size: 32);

create: (MemAlloc mem) -> () => (out: &linked_list) =
{
  out = mem->allocate(sizeof linked_list);
  out.size = 0;
  out.start = create();
}

ll_node: (next: &ll_node, value: 32);

create: (MemAlloc mem) -> () => (out: &ll_node) =
{
  out = mem->allocate(sizeof ll_node);
}

ll : &linked_list = create();
curr_node := ll.start;
// Creates a linked list where index equals value over range r1 (0 to 200)
// "else" handles edge cases for loops
for r1 {
  curr_node.value = r1;
  curr_node.next = create();
  curr_node = curr_node.next;
} else {
  curr_node.value = r1;
}

// The range below starts at ll*.start. Then it applies a dereference,
// and accesses the member "next" at each step. Finally it end the range,
// when ll.start.next...(.next~) == NULL
r5 = ll.start..(.next)...NULL; // Equivalent to r1

println# $r1
println# $r2
println# $r3
println# $r4
println# $r5
