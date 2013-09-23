for (p in 2:4) {
    for (constr in c("none", "all", -0.1, -0.2)) {
        carg <- ifelse(constr == "all", "c", "!c");
        larg <- ifelse(constr == "none" || constr == "all", "!L", paste(sep="", "L=", constr));
        for (dist in c("beta55", "even")) {
            for (v in c(999)) for (i in c(1000)) for (f in c(0.01)) for (I in c(0.1)) for (D in 0.05) for (A in c(F)) for (e in c("periodic")) for (E in c(10)) {
                pat <- paste(sep="",
                             "^elections:p",p,",",
                             carg, ",", larg, ",",
                             "v",v,",d",dist,",i",i,",F",f,",I",I,",D",D,",",ifelse(A,"A","!A"),
                             ",e",e,",E",E,",",
                             "seed=[0-9]+\\.data$");
                rfs <- list.files("results/",
                                  pattern=pat);

                for (file in rfs) {
                    d <- read.
                }
            }
        }
    }
}

