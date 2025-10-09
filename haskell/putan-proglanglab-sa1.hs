-- Luis Gabrielle Putan
-- Principles of Programming Languages Laboratory
-- SA1


-- factorial function
factorialOf :: Integer -> Integer
-- pattern matching
factorialOf 0 = 1 -- base case of the function
factorialOf n = n * factorialOf (n - 1) -- recursive step

-- computation for the win combinations
winCombinations :: Integer -> Integer -> Integer
winCombinations totalNum draws = factorialOf totalNum `div` (factorialOf draws * factorialOf (totalNum - draws))  

main = do
    putStrLn ("Number of possible winning combinations: " ++ show (winCombinations 42 6))

