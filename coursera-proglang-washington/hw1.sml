fun is_older (date1 : int * int * int, date2 : int * int * int) =
    let
        val (y1, m1, d1) = date1
        val (y2, m2, d2) = date2
    in
        if y1 < y2 then true
        else if y1 > y2 then false
        else if m1 < m2 then true
        else if m1 > m2 then false
        else d1 < d2
    end

(* count the number of dates in a list that fall in a given month *)
fun number_in_month (date : (int * int * int) list, month : int) =
    let
        fun count (date : (int * int * int) list, month : int, date_count : int) =
            if date = [] then date_count
            else if #2 (hd date) = month then count (tl date, month, date_count + 1)
            else count (tl date, month, date_count)
    in
        count (date, month, 0)
    end

fun number_in_months (date : (int * int * int) list, months : int list) =
    let
        fun count (date : (int * int * int) list, months : int list, date_count : int) =
            if months = [] then date_count
            else count (date, tl months, date_count + number_in_month(date, hd months))
    in
        count (date, months, 0)
    end

fun dates_in_month (date : (int * int * int) list, month : int) =
    let
        fun in_month (date : (int * int * int) list, month : int, date_list : (int * int * int) list) =
            if date = [] then date_list
            else if #2 (hd date) = month then in_month (tl date, month, date_list @ [hd date])
            else in_month (tl date, month, date_list)
    in
        in_month (date, month, [])
    end

fun dates_in_months (date : (int * int * int) list, months : int list) =
    let
        fun in_months (date : (int * int * int) list, months : int list, date_list : (int * int * int) list) =
            if months = [] then date_list
            else in_months (date, tl months, date_list @ dates_in_month(date, hd months))
    in
        in_months (date, months, [])
    end

fun get_nth(strings : string list, n : int) =
    let
        fun get_nth(strings : string list, n : int, index : int) =
            if index = n then hd strings
            else get_nth(tl strings, n, index + 1)
    in
        get_nth(strings, n, 1)
    end

fun date_to_string(date : int * int * int) =
    let
        val months = ["January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"]
        val (y, m, d) = date
        val month = get_nth(months, m)
        val concatenated = month ^ " " ^ Int.toString(d) ^ ", " ^ Int.toString(y)
    in
        concatenated
    end

fun number_before_reaching_sum(sum : int, numbers : int list) = 
    let
        fun running_sum(numbers : int list, sum_numbers : int, count : int) =
            if sum_numbers >= sum then count-1
            else running_sum(tl numbers, sum_numbers + hd numbers, count+1)
    in
        running_sum(numbers, 0, 0)
    end

fun what_month(day_year : int) = 
    let 
        val months = [31,28,31,30,31,30,31,31,30,31,30,31]
        val month = number_before_reaching_sum(day_year, months) + 1
    in
        month
    end

fun month_range(day1 : int, day2 : int) = 
    let 

        fun months_range(day1 : int, month_list : int list) =
            if day1 > day2 then month_list
            else months_range(day1+1, month_list @ [what_month(day1)])
    in
        months_range(day1, [])
    end

fun oldest(dates: (int*int*int) list) = 
    let 
        fun oldest(dates: (int*int*int) list, oldest_date : (int*int*int)) =
            if dates = [] then oldest_date
            else if is_older(hd dates, oldest_date) then oldest(tl dates, hd dates)
            else oldest(tl dates, oldest_date)
    
    in
        if dates = [] then NONE
        else SOME (oldest(dates, hd dates))
    end