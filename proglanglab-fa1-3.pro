
% base case
sum([], 0).

% recursive case
sum(L, S) :- 
    L = [ListHead | ListTail],
    sum(ListTail, TailSum),
    S is ListHead + TailSum.

