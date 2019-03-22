
(module

    ;; io js functions
    (import "console" "read_char" (func $read_char_js (result i32)))
    (import "console" "read_num" (func $read_num_js (result i32)))
    (import "console" "write_char" (func $write_char_js (param i32)))
    (import "console" "write_num" (func $write_num_js (param i32)))


    (memory 2)

    (global $memory_page_size i32 i32.const 65636)
    (global $stack_heap_wall i32 i32.const 65536)
    (global $stack_pointer (mut i32) i32.const 65536)


    (func $check_heap_bounds (param $address i32) (result i32)
        get_local $address
        get_global $stack_heap_wall
        i32.ge_s
        if unreachable end
        get_local $address

    )

    (func $check_stack_bounds (param $address i32) (result i32)
        get_local $address
        get_global $stack_heap_wall
        i32.lt_s
        if unreachable end
        get_local $address
    )


    ;; Stack Manipulation
    (func $push (param $value i32)
        get_global $stack_pointer
        get_local $value
        i32.store

        get_global $stack_pointer
        i32.const 4
        i32.add
        set_global $stack_pointer

        ;; expand memory if needed
        get_global $stack_pointer
        current_memory
        get_global $memory_page_size
        i32.mul
        i32.ge_s
        if
            i32.const 1
            grow_memory
            drop
        end
    )

    (func $pop (result i32)
        get_global $stack_pointer
        i32.const 4
        i32.sub
        set_global $stack_pointer

        get_global $stack_pointer
        call $check_stack_bounds

        i32.load
    )

    (func $duplicate
        get_global $stack_pointer
        i32.const 4
        i32.sub
        call $check_stack_bounds
        i32.load
        call $push
    )

    (func $copy (param $offset i32)
        get_global $stack_pointer
        get_local $offset
        i32.const 4
        i32.mul
        i32.sub
        i32.const 4
        i32.sub
        call $check_stack_bounds
        i32.load
        call $push

    )

    (func $swap (local $holder i32)
        call $pop
        set_local $holder
        call $pop
        get_local $holder
        call $push
        call $push
    )

    (func $discard
        call $pop
        drop
    )

    (func $slide (param $offset i32)
        call $pop

        get_global $stack_pointer
        get_local $offset
        i32.const 4
        i32.mul
        i32.sub
        call $check_stack_bounds
        set_global $stack_pointer
        call $push
    )


    ;; Arithmetic
    (func $add
        call $pop
        call $pop
        i32.add
        call $push
    )

    (func $sub
        call $swap
        call $pop
        call $pop
        i32.sub
        call $push
    )

    (func $mul
        call $pop
        call $pop
        i32.mul
        call $push
    )

    (func $div
        call $swap
        call $pop
        call $pop
        i32.div_s
        call $push
    )

    (func $mod
        call $swap
        call $pop
        call $pop
        i32.rem_s
        call $push
    )

    ;; Heap Access
    (func $store (param $address i32) (param $data i32)
        get_local $address
        i32.const 4
        i32.mul
        call $check_heap_bounds
        get_local $data
        i32.store
    )

    (func $load (param $address i32) (result i32)
        get_local $address
        i32.const 4
        i32.mul
        call $check_heap_bounds
        i32.load
    )

    ;; I/O
    (func $read_char (param $address i32)
        get_local $address
        i32.const 4
        i32.mul
        call $check_heap_bounds
        call $read_char_js
        i32.store
    )

    (func $read_num (param $address i32)
        get_local $address
        i32.const 4
        i32.mul
        call $check_heap_bounds
        call $read_num_js
        i32.store
    )

    (func $write_char
        call $pop
        call $write_char_js
    )

    (func $write_num
        call $pop
        call $write_num_js
    )

    

    ;; MAIN
    (func (export "main")
        block $mainBlock
i32.const 0
call $push
i32.const 10
call $push
i32.const 33
call $push
i32.const 100
call $push
i32.const 108
call $push
i32.const 114
call $push
i32.const 111
call $push
i32.const 87
call $push
i32.const 32
call $push
i32.const 111
call $push
i32.const 108
call $push
i32.const 108
call $push
i32.const 101
call $push
i32.const 72
call $push
end

block $1
loop $1_l
call $duplicate
call $pop
i32.eqz
br_if $1
call $write_char
br $1_l
end
end

block $2
end


    )
)
