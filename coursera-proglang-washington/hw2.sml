(* Dan Grossman, Coursera PL, HW2 Provided Code *)

(* helper for string equality *)
fun same_string(str1 : string, str2 : string) =
    str1 = str2

(* problem 1 *)
fun all_except_option (target, []) = NONE
  | all_except_option (target, current :: rest) =
    case same_string(target, current) of
         true => SOME rest
       | false =>
            case all_except_option(target, rest) of
                 NONE => NONE
               | SOME remaining => SOME (current :: remaining)
				    
(* this doesn't seem to be functioning quite right *)
fun get_substitutions1 ([], base) = []
  | get_substitutions1 (first_list :: remaining_lists, base) =
    case all_except_option(base, first_list) of
         NONE => []
       | SOME substitutions =>
            case get_substitutions1(remaining_lists, base) of
                 [] => substitutions
               | others => substitutions @ others
						 
fun get_substitutions2 (lists, base) =
    let
        fun helper (base, [], acc) = acc
          | helper (base, first :: rest, acc) =
            case all_except_option(base, first) of
                 NONE => helper(base, rest, acc)
               | SOME subs => helper(base, rest, subs @ acc)
    in
        helper(base, lists, [])
    end

type fullname = {first: string, middle: string, last: string}

fun similar_names (substitutions, {first = fname, middle = mname, last = lname}) =
    let
        val base_name = {first = fname, middle = mname, last = lname}
        fun make_subs(names) =
            case names of
                 [] => []
               | alt :: rest => {first = alt, middle = mname, last = lname} :: make_subs(rest)
    in
        [base_name] @ make_subs(get_substitutions2(substitutions, fname))
    end
	   
		 				 
(* problem 2 *)

datatype suit = Clubs | Diamonds | Hearts | Spades
datatype rank = Jack | Queen | King | Ace | Num of int 
type card = suit * rank

datatype color = Red | Black
datatype move = Discard of card | Draw 

exception IllegalMove

fun card_color (c : suit * rank) =
    case c of
         (Clubs, _) => Black
       | (Spades, _) => Black
       | _ => Red
		      
fun card_value (c : suit * rank) =
    case c of
         (_, Ace) => 11
       | (_, Num n) => n
       | _ => 10
 		       
fun remove_card ([], card_to_remove : card, ex) = raise ex
  | remove_card (card_head :: card_tail, card_to_remove, ex) =
    case card_head = card_to_remove of
         true => card_tail
       | false => card_head :: remove_card(card_tail, card_to_remove, ex)
		    
fun all_same_color [] = true
  | all_same_color (first_card :: rest_cards) = 
    let
        val ref_color = card_color(first_card)
        fun check_colors [] = true
          | check_colors (c :: cs) =
            case card_color(c) = ref_color of
                 false => false
               | true => check_colors(cs)
    in
        check_colors(rest_cards)
    end
	    
fun sum_cards (cards : card list) =
    let
        fun sum_helper ([], total) = total
          | sum_helper (c :: cs, total) =
                sum_helper(cs, card_value(c) + total)
    in
        sum_helper(cards, 0)
    end
	
val sum_cards_test = sum_cards([(Spades, Ace), (Hearts, Num 4)]) = 15;

fun score ([], goal) = goal
  | score (cards : card list, goal : int) = 
    let
        val total = sum_cards(cards)
        val base_score =
            if total <= goal
            then goal - total
            else 3 * (total - goal)
    in
        if all_same_color(cards)
        then base_score div 2
        else base_score
    end
	
fun officiate (deck : card list, moves : move list, goal : int) =
    let
        fun play (hand, deck, [], goal) = score(hand, goal)
          | play (hand, [], moves, goal) = score(hand, goal)
          | play (hand : card list, deck, moves, goal) =
            if sum_cards(hand) > goal then score(hand, goal)
            else
                case moves of
                     [] => score(hand, goal)
                   | mv :: rest_moves =>
                        case mv of
                             Discard cd =>
                                 play(remove_card(hand, cd, IllegalMove), deck, rest_moves, goal)
                           | Draw =>
                                 case deck of
                                      [] => score(hand, goal)
                                    | next_card :: remaining_deck =>
                                          play(next_card :: hand,
                                               remove_card(deck, next_card, IllegalMove),
                                               rest_moves,
                                               goal)
    in
        play([], deck, moves, goal)
    end

fun check_num_aces (cards, acc) =
    let
        fun count_aces ([], total) = total
          | count_aces ((_, rank_val) :: rest, total) =
                case rank_val of
                     Ace => count_aces(rest, total + 1)
                   | _ => count_aces(rest, total)
    in
        count_aces(cards, 0)
    end

fun lowest_score (cards, processed, best, goal) =
    case cards of
         [] => best
       | (suit_val, Ace) :: rest_cards =>
            let
                val ace_as_one = processed @ [(suit_val, Num 1)] @ rest_cards
                val alt_score = score(ace_as_one, goal)
            in
                if alt_score < best
                then lowest_score(rest_cards, processed @ [(suit_val, Num 1)], alt_score, goal)
                else lowest_score(rest_cards, processed @ [(suit_val, Ace)], best, goal)
            end
       | card_head :: rest_cards =>
            lowest_score(rest_cards, processed @ [card_head], best, goal)
			                
fun score_challenge (cards, goal) =
    let
        val normal_score = score(cards, goal)
    in
        lowest_score(cards, [], normal_score, goal)
    end

fun officiate_challenge (deck, moves, goal) =
    let
        fun simulate (deck, hand, moves) =
            case moves of
                 [] => hand
               | Discard c :: rest_moves =>
                     simulate(deck, remove_card(hand, c, IllegalMove), rest_moves)
               | Draw :: rest_moves =>
                     case deck of
                          [] => hand
                        | next_card :: remaining_deck =>
                              if (card_value(next_card) + sum_cards(hand)) > goal
                              then next_card :: hand
                              else simulate(remaining_deck, next_card :: hand, rest_moves)
    in
        score_challenge(simulate(deck, [], moves), goal)
    end
	
fun discard_check (hand, card_to_check, goal) = 
    let
        val diff = goal - sum_cards(card_to_check :: hand)
        fun search_for_match ([], diff) = NONE
          | search_for_match (h :: rest, diff) =
                if card_value(h) = diff
                then SOME h
                else search_for_match(rest, diff)
    in
        search_for_match(hand, diff)
    end
	
fun careful_player (deck, goal) =
    let
        fun decide_moves (moves, hand, [], goal) = moves
          | decide_moves (moves, hand, c :: rest, goal) =
                case score(hand, goal) of 
                     0 => moves
                   | _ =>
                        case discard_check(hand, c, goal) of
                             NONE => decide_moves(Draw :: moves, c :: hand, rest, goal)
                           | SOME match_card =>
                                 decide_moves(Draw :: Discard match_card :: moves,
                                              c :: remove_card(hand, match_card, IllegalMove),
                                              rest,
                                              goal)
    in
        decide_moves([], [], deck, goal)
    end
