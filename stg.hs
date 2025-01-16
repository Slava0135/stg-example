one = {} \n {} -> Int# {1#}
two = {} \n {} -> Int# {2#}
one_hundred = {} \n {} -> Int# {100#}

plus = {} \n {l,r} -> case l {} of
    Int# {lI} -> case r {} of
        Int# {rI} -> case +# {lI,rI} of
            resI -> Int# {resI}



mult = {} \n {l,r} -> case l {} of
    Int# {lI} -> case r {} of
        Int# {rI} -> case *# {lI,rI} of
            resI -> Int# {resI}



sub = {} \n {l,r} -> case l {} of
    Int# {lI} -> case r {} of
        Int# {rI} -> case -# {lI,rI} of
            resI -> Int# {resI}



eq = {} \n {l,r} -> case l {} of
    Int# {lI} -> case r {} of
        Int# {rI} -> case ==# {lI,rI} of
            1# -> True {}
            _ -> False {}



id_eq = {} \n {l_id,r_id} -> case l_id {} of
    VarId {l} -> case r_id {} of
        VarId {r} -> eq l r



eval = {} \n {valueOf,expr} ->
    letrec go = {valueOf,go} \n {expr} -> case expr {} of
        Lit {n} -> n {}
        Var {x} -> valueOf {x}
        Add {l,r} ->
            let go_l = {go,l} \u {} -> go {l}
                go_r = {go,r} \u {} -> go {r}
            in plus {go_l,go_r}
        Mul {l,r} ->
            let go_l = {go,l} \u {} -> go {l}
                go_r = {go,r} \u {} -> go {r}
            in mult {go_l,go_r}
        Let {x,e,body} ->
            letrec
                n = {go,e} \u {} -> go {e}
                valueOf' = {x,n,valueOf} \n {y} -> case id_eq {x,y} of
                    True {} -> n {}
                    _ -> valueOf {y}
            in eval {valueOf',body}
    in go {expr}


pow = {} \n {e,n} -> case n {} of
    Int# {nI} -> case nI of
        0# -> Lit {one}
        _ ->
            letrec
                n' = {n} \u {} -> sub {n,one}
                pow' = {e,n'} \u {} -> pow {e,n'}
            in Mul {e pow'}


sop = {} \n {e,n} -> case n {} of
    Int# {nI} -> case nI of
        0# -> Lit {one}
        _ ->
            letrec
                z = {n} \n {} -> VarId {n}
                n' = {n} \u {} -> sub n one;
                sop' = {e,n'} \u {} -> sop {e,n'}
                var_z = {z} \n {} -> Var {z}
                add = {var_z,sop'} \n {} Add {var_z,sop'}
                pow' = {e,n} \u {} -> pow {e,n}
            in Let {z,pow',add}


main = {} \n {} ->
    letrec
        x = {} \n {} -> VarId {one_hundred}
        var_x = {x} \n {} -> Var {x}
        lit_one = {} \n {} -> Lit {one}
        add = {var_x,lit_one} \n {} -> Add {var_x,lit_one}
        e = {add} \u {} -> sop {add,two}
        valueOf = {} \n {varid_i} -> case varid_i {} of
            VarId {i} -> case i {} of
                Int# iI -> case iI of
                    100# -> Int# {8#}
                    _ -> Int# {0#}
    in eval {valueOf,e}


