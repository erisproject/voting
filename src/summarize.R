for (p in 3:4) {
    for (constr in c("none", "all", -0.1, -0.2)) {
        carg <- ifelse(constr == "all", "c", "!c");
        larg <- ifelse(constr == "none" || constr == "all", "!L", paste(sep="", "L=", constr));
        for (dist in c("beta55")) {
            for (v in c(999)) for (i in c(1000)) for (f in c(0.01)) for (I in c(0.1)) for (D in 0.05) for (A in c(F)) for (e in c("periodic")) for (E in c(10)) {
                opts <- paste(sep="",
                              "p",p,",",
                              carg, ",", larg, ",",
                              "v",v,",d",dist,",i",i,",F",f,",I",I,",D",D,",",
                              ifelse(A,"A","!A"), ",e",e,",E",E,",");
                pat <- paste(sep="", "^elections:", opts, "seed=[0-9]+\\.data$");

                rfs <- list.files("results/",
                                  pattern=pat);
                
                if (length(rfs) > 0) {
                  cat(opts, "\n");
                  means <- c();
                  meansmL <- c();
                  freqmL <- c();
                  for (file in rfs) {
                    data <- read.csv(paste(sep="/", "results", file));
                    means[length(means)+1] <- mean(data$position);
                    nonleft <- data[data$winner != 1,]$position;
                    
                    meansmL[length(meansmL)+1] <- mean(nonleft);
                    freqmL[length(freqmL)+1] <- length(nonleft) / nrow(data);
                    
                    
                    #print(nrow(data));
                    #print(names(data));
                    
                  }
                  cat("p", p, "constr", constr, "&", paste(sep=" & ", c(mean(means), sd(means), fivenum(means)), collapse=" & "), "\n");
                  
                  cat("w/o left: p", p, "constr", constr, "&", paste(sep=" & ", collapse=" & ", c(mean(freqmL), mean(meansmL), sd(meansmL), fivenum(meansmL))), "\n");

                  cat("\n\n");
                }
            }
        }
    }
}

