set.seed(1234);

even <- 0:998 / (998/2) - 1;
unif <- runif(999, -1, 1);
beta22 <- rbeta(999, 2, 2) * 2 - 1;
beta55 <- rbeta(999, 5, 5) * 2 - 1;

pdf("dists.pdf", width=3.25, height=3.5);
par("mar", c(5.1,4.1,4.1,5));
hist(even, -25:25/25, ylim=c(0,1.5), freq=FALSE);
hist(unif, -25:25/25, ylim=c(0,1.5), freq=FALSE);
plot.function(function (x) dunif(x, -1, 1), from=-1, to=1, ylim=c(0,1.5), add=TRUE);
hist(beta22, -25:25/25, ylim=c(0,1.5), freq=FALSE);
plot.function(function (x) dbeta((x+1)/2, 2,2)/2, from=-1, to=1, ylim=c(0,1.5), add=TRUE);
hist(beta55, -25:25/25, ylim=c(0,1.5), freq=FALSE);
plot.function(function (x) dbeta((x+1)/2, 5,5)/2, from=-1, to=1, ylim=c(0,1.5), add=TRUE);
dev.off();
