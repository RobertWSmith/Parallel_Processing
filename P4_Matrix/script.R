library(sqldf)
library(ggplot2)
library(gridExtra)

P4mtx <- read.table(   
  "~/GitHub/P4_Matrix/P4_mtx.csv",   
  header = FALSE,
  sep = ",",   
  colClasses = c("factor", "numeric") 
) 

P4mtx <- cbind(P4mtx, rep("P4", nrow(P4mtx)))
names(P4mtx) <- c("block_size", "time", "problem")
P4mtx[,2] <- P4mtx[,2]/1000

minsP4mtx <- sqldf("select block_size, min(time) as time from P4mtx group by block_size")

ggplot(minsP4mtx) + geom_histogram(aes(x= block_size, y = time))

ggplot(P4mtx) + geom_boxplot(aes(x = block_size, y = time))

anova <- aov(time~block_size, P4mtx)
summary(anova)

TukeyHSD(anova)

P4Floyd <- read.table(
  "~/GitHub/P4_Floyd/P3.csv",
  header = FALSE,
  sep = ",",
  colClasses = c("factor", "numeric")
)

P4Floyd <- cbind(P4mtx, rep("P4", nrow(P4Floyd)))
names(P4Floyd) <- c("block_size", "time", "problem")
P4mtx[,2] <- P4Floyd[,2]/1000

anova <- aov(time~block_size, P4Floyd)
summary(anova)

TukeyHSD(anova)

P3Floyd <- read.table(
  "~/GitHub/Parallel_Processing/FloydGraph/P3.csv",
  header = TRUE,
  sep = ",",
  colClasses = c("factor", "numeric")
)

P3Floyd <- cbind(P4mtx, rep("P3", nrow(P3Floyd)))
names(P3Floyd) <- c("block_size", "time", "problem")

anova <- aov(time~block_size, P3Floyd)
summary(anova)

TukeyHSD(anova)

P2mtx <- read.table(
  "~/GitHub/Parallel_Processing/Program2/P2_output.csv",
  header = FALSE,
  sep = ",", 
  colClasses = c("factor", "numeric", "numeric")
)

P2mtx <- P2mtx[,1:2]
P2mtx <- cbind(P2mtx, rep("P2", nrow(P2mtx)))
names(P2mtx) <- c("block_size", "time", "problem")

anova <- aov(time~block_size, P2mtx)
summary(anova)

TukeyHSD(anova)


mtx <- as.data.frame(rbind(P2mtx, P4mtx))
mtx[,3] <- as.factor(mtx[,3])

anova <- aov(time~block_size + problem, mtx)
summary(anova)
TukeyHSD(anova)

floyd <- as.data.frame(rbind(P3Floyd, P4Floyd))
floyd[,3] <- as.factor(floyd[,3])

anova <- aov(time~block_size + problem, floyd)
summary(anova)
TukeyHSD(anova)



