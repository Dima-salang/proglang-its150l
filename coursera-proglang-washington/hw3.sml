(* Coursera Programming Languages, Homework 3, Provided Code *)

exception NoAnswer

datatype pattern =
      Wildcard
    | Variable of string
    | UnitP
    | ConstP of int
    | TupleP of pattern list
    | ConstructorP of string * pattern

datatype valu =
      Const of int
    | Unit
    | Tuple of valu list
    | Constructor of string * valu

fun g func_wild func_var pat_expr =
    let 
        val recur = g func_wild func_var 
    in
        case pat_expr of
            Wildcard          => func_wild ()
          | Variable var_name => func_var var_name
          | TupleP pat_list   => List.foldl (fn (p_elem, acc) => (recur p_elem) + acc) 0 pat_list
          | ConstructorP(_, p_sub) => recur p_sub
          | _ => 0
    end


(**** for the challenge problem only ****)

datatype typ =
      Anything
    | UnitT
    | IntT
    | TupleT of typ list
    | Datatype of string


(**** you can put all your code here ****)

fun only_capitals (str_list : string list) =
    List.filter (fn s => Char.isUpper (String.sub (s, 0))) str_list

fun longest_string1 (str_list : string list) = 
   List.foldl (fn (curr, best) =>
        if String.size curr > String.size best then curr else best)
        "" str_list

fun longest_string2 (str_list : string list) = 
   List.foldl (fn (curr, best) =>
        if String.size curr >= String.size best then curr else best)
        "" str_list

fun longest_string_helper cmp_fn =
    fn str_list =>
        List.foldl (fn (s1, s2) =>
            if cmp_fn (String.size s1, String.size s2) then s1 else s2)
            "" str_list

val longest_string3 = fn strs => longest_string_helper (fn (x, y) => x > y) strs
val longest_string4 = fn strs => longest_string_helper (fn (x, y) => x >= y) strs

val longest_capitalized = longest_string1 o only_capitals
						
val rev_string = String.implode o List.rev o String.explode

fun first_answer func =
    fn alist =>
        case alist of
            [] => raise NoAnswer
          | head :: tail =>
                case func head of
                    SOME res => res
                  | NONE => first_answer func tail

fun all_answers func =
    let
        fun aux (f, lst, accum) =
            case lst of
                [] => SOME []
              | item :: rest =>
                    (case f item of
                         NONE => NONE
                       | SOME vals => aux (f, rest, [vals] @ accum))
    in
        fn al => aux (func, al, [])
    end
	
fun count_wildcards pat_expr =
    g (fn () => 1) (fn _ => 0) pat_expr

fun count_wild_and_variable_lengths pat_expr =
    g (fn () => 1) String.size pat_expr

fun count_some_var (target, pat_expr) =
    g (fn () => 0) (fn var => if target = var then 1 else 0) pat_expr

fun check_pat pat_expr =
    let
        fun extract_vars pat =
            case pat of
                Variable s => [s]
              | TupleP pats => List.foldl (fn (p, acc) => (extract_vars p) @ acc) [] pats
              | ConstructorP (_, p_inner) => extract_vars p_inner
              | _ => []
        fun has_duplicates sl =
            case sl of
                [] => false
              | x :: xs => if List.exists (fn y => x = y) xs then true else has_duplicates xs
        val validate = not o has_duplicates o extract_vars
    in
        validate pat_expr
    end


fun match (val_expr, pat_expr) =
    case (val_expr, pat_expr) of
        (_, Wildcard) => SOME []
      | (v, Variable s) => SOME [(v, s)]
      | (Unit, UnitP) => SOME []
      | (Const c_val, ConstP p_val) => if c_val = p_val then SOME [] else NONE
      | (Tuple v_list, TupleP p_list) =>
            if List.length v_list = List.length p_list
            then all_answers match (ListPair.zip (v_list, p_list))
            else NONE
      | (Constructor (name_v, val_inner), ConstructorP (name_p, pat_inner)) =>
            if name_v = name_p then match (val_inner, pat_inner) else NONE
      | _ => NONE


fun first_match v_expr =
    fn pat_list =>
        SOME (first_answer (fn pat => match (v_expr, pat)) pat_list)
        handle NoAnswer => NONE
