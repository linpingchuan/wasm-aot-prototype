;; RUN: sexpr-dump %s > %t1 | %sexpr-wasm %t1 > %t2
;; RUN: sexpr-dump %t1 > %t3
;; RUN: diff %t1 %t3
(module
  ;; unnamed
  (import "foo" "bar" (param i32) (result i64))

  ;; named
  (import $print_i32 "stdio" "print" (param i32))
  (import $add_i32 "math" "add" (param i32 i32) (result i32))
  (import $f32 "test" "f32" (param f32) (result f32))
  (import $f64 "test" "f64" (param f64) (result f64))
  (import $i64 "test" "i64" (param i64) (result i64)))