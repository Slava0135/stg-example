data Int = Int# Int#
data VarId = VarId Int deriving (Eq)
data Expr
    = Lit Int
    | Var VarId
    | Add Expr Expr
    | Mul Expr Expr
    | Let VarId Expr Expr

eval :: (VarId -> Int) -> Expr -> Int
eval valueOf = go
    where
        go (Lit n) = n
        go (Var x) = valueOf x
        go (Add l r) = go l + go r
        go (Mul l r) = go l * go r
        go (Let x e body) =
            let n = go e
                valueOf' y = if x == y then n else valueOf y
            in eval valueOf' body

pow :: Expr -> Int -> Expr
pow e 0 = Lit 1
pow e n = Mul e (pow e (n - 1))

sop :: Expr -> Int -> Expr
sop e 0 = Lit 1
sop e n =
    let z = VarId n
    in Let z (pow e n) (Add (Var z) (sop e (n - 1)))

main :: Int
main =
    let x = VarId 100
        e = sop (Add (Var x) (Lit 1)) 2
        valueOf (VarId i) =
            case i of
                100 -> 8
                _ -> 0
    in eval valueOf e
