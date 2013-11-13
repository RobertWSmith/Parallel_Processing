
mtx <- read.table(
  "~/GitHub/P4_Matrix/P4_mtx.csv",
  header=F, 
  sep = ",",
  colClasses = c("factor", "numeric")
)
names(mtx) <- c("block_size", "time")
mtx[,2] <- mtx[,2]/1000

anova <- aov(time~block_size, mtx)

summary(anova)
plot(anova)


(tukey <- TukeyHSD(anova))

summary(tukey)
plot(tukey)



