one = \ -> Int# 1#;
two = \ -> Int# 2#;
one_hundred = \ -> Int# 100#;

plus = \l r -> case l of
    Int# lI -> case r of
        Int# rI -> case +# lI rI of
            resI -> Int# resI;
        _ -> PlusError;
    _ -> PlusError;

mult = \l r -> case l of
    Int# lI -> case r of
        Int# rI -> case *# lI rI of
            resI -> Int# resI;
        _ -> MultError;
    _ -> MultError;

sub = \l r -> case l of
    Int# lI -> case r of
        Int# rI -> case -# lI rI of
            resI -> Int# resI;
        _ -> MultError;
    _ -> MultError;

eq = \l r -> case l of
    Int# lI -> case r of
        Int# rI -> case ==# lI rI of
            1# -> True;
            _ -> False;
        _ -> EqError;
    _ -> EqError;

id_eq = \l_id r_id -> case l_id of
    VarId l -> case r_id of
        VarId r -> eq l r;
        _ -> IdEqError;
    _ -> IdEqError;

eval = \valueOf expr ->
    letrec go = \(valueOf go) expr -> case expr of
            Lit n -> n;
            Var x -> valueOf x;
            Add l r ->
                let go_l = \(go l) => go l;
                    go_r = \(go r) => go r
                in plus go_l go_r;
            Mul l r ->
                let go_l = \(go l) => go l;
                    go_r = \(go r) => go r
                in mult go_l go_r;
            Let x e body ->
                letrec
                    n = \(go e) => go e;
                    valueOf' = \(x n valueOf) y -> case id_eq x y of
                        True -> n;
                        _ -> valueOf y
                in eval valueOf' body;
            _ -> EvalError
    in go expr;

pow = \e n -> case n of
    Int# nI -> case nI of
        0# -> Lit one;
        _ ->
            letrec
                n' = \(n) => sub n one;
                pow' = \(e n') => pow e n'
            in Mul e pow';
    _ -> PowError;

sop = \e n -> case n of
    Int# nI -> case nI of
        0# -> Lit one;
        _ ->
            letrec
                z = \(n) -> VarId n;
                n' = \(n) => sub n one;
                sop' = \(e n') => sop e n';
                var_z = \(z) -> Var z;
                add = \(var_z sop') -> Add var_z sop';
                pow' = \(e n) => pow e n
            in Let z pow' add;
    _ -> SopError;

main = \ ->
    letrec
        x = \ -> VarId one_hundred;
        var_x = \(x) -> Var x;
        lit_one = \ -> Lit one;
        add = \(var_x lit_one) -> Add var_x lit_one;
        e = \(add) => sop add two;
        valueOf = \varid_i -> case varid_i of
            VarId i -> case i of
                Int# iI -> case iI of
                    100# -> Int# 8#;
                    _ -> Int# 0#;
                _ -> MainError;
            _ -> MainError
    in eval valueOf e
