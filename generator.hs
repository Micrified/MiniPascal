module Main where
    import System.IO
    import System.Environment
    import System.Exit

    -- English Alphabet.
    alphabet :: [String]
    alphabet = map (\x -> [x]) "abcdefghijklmnopqrstuvwxyz"

    -- Infinite generator of all identifier permutations.
    identifiers :: [String]
    identifiers = moreIdentifiers alphabet
        where
            moreIdentifiers :: [String] -> [String]
            moreIdentifiers xs = let zs = [x ++ y | x <- xs, y <- alphabet] in zs ++ moreIdentifiers zs

    -- Reads integer argument from command arg vector.
    getIntArg :: IO Int
    getIntArg = fmap (read . head) getArgs

    main :: IO ()
    main = do arg <- getIntArg; mapM print (take arg identifiers); return ();
