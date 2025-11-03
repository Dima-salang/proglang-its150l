use "hw2.sml";
(* Homework2 Simple Test *)
(* These are basic test cases. Passing these tests does not guarantee that your code will pass the actual homework grader *)
(* To run the test, add a new line to the top of this file: use "homeworkname.sml"; *)
(* All the tests should evaluate to true. For example, the REPL should say: val test1 = true : bool *)

val test1 = all_except_option ("string", ["string"])

val test2 = get_substitutions1 ([["foo"],["there"]], "foo")

val test3 = get_substitutions2 ([["foo"],["there"]], "foo") 

val test4 = similar_names ([["Fred","Fredrick"],["Elizabeth","Betty"],["Freddie","Fred","F"]], {first="Fred", middle="W", last="Smith"})

val test5 = card_color (Clubs, Num 2)

val test6 = card_value (Clubs, Num 2)

val test7 = remove_card ([(Hearts, Ace)], (Hearts, Ace), IllegalMove)

val test8 = all_same_color [(Hearts, Ace), (Hearts, Ace)]

val test9 = sum_cards [(Clubs, Num 2),(Clubs, Num 2)]

val test10 = score ([(Hearts, Num 2),(Clubs, Num 4)],10)

val test11 = officiate ([(Hearts, Num 2),(Clubs, Num 4)],[Draw], 15)

val test12 = officiate ([(Clubs,Ace),(Spades,Ace),(Clubs,Ace),(Spades,Ace)],
                        [Draw,Draw,Draw,Draw,Draw],
                        42)

val test13 = ((officiate([(Clubs,Jack),(Spades,Num(8))],
                         [Draw,Discard(Hearts,Jack)],
                         42);
               false) 
              handle IllegalMove)
             
             