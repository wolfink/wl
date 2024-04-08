//typedef int8_t   i8;
//typedef int16_t  i16;
//typedef int32_t  i32;
//typedef int64_t  i64;
//typedef uint8_t  u8;
//typedef uint16_t u16;
//typedef uint32_t u32;
//typedef uint64_t u64;
//typedef i8       b8;
//typedef float    f32;
//typedef double   f64;

//#define WLC_STMT(S) { S }
//#define WLC_ASSERT(c) WLC_STMT( if(!(c)) { *(int*)0 = 0; } )

#define KB(num) 1024 * num
#define MB(num) KB(1) * KB(1) * num
#define GB(num) MB(1) * MB(1) * num

#define NULL_CHECK(obj, fun)\
if (obj == NULL) die("error: " #fun ": " #obj " is NULL\n");
