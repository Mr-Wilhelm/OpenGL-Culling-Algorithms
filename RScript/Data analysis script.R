library(ggplot2)

dataCSV <- read.csv("Environ1Data.csv")
xValue <- dataCSV[, 2]
yValue <- dataCSV[, 5]
df <- data.frame(xValue,yValue)

ggplot(df, aes(x = xValue, y = yValue)) + geom_line() +
  ggtitle("Graph to show Polygon count against RAM usage") +
  xlab("Polygon Count") + 
  ylab("RAM usage")

res.aov <- aov(Algorithm ~ PolCount + RAM, data = dataCSV)
summary(res.aov)



#print(cat("polygons are rendered per unit of RAM", dataCSV[1, 2] / dataCSV[1, 5]))
#print(cat("polygons are rendered per unit of GPU usage", dataCSV[1, 2] / dataCSV[1, 4]))

#print(cat("frames per second are rendered per unit of RAM", dataCSV[1, 3] / dataCSV[1, 5]))
#print(cat("frames per second are rendered per unit of GPU usage", dataCSV[1, 3] / dataCSV[1, 4]))

#plot(x = dataCSV$RAM, y = dataCSV$PolCount,
#xlab = "Ram Usage",
#ylab = "PolCount",
#main = "Polygon Count against Ram Usage"
#)

