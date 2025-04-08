library(dplyr)
library(ggplot2)
library(scales)

#Code for the data is a mix of basic RStudio code, and libraries. The library code is explained below.

# "%>%" is the pipe operator. Allows you to do stuff to the variable assigned on that line, makes code neater
# https://magrittr.tidyverse.org/reference/pipe.html

# "mutate()" is a dplyr function, that adds new variables to existing data
# https://dplyr.tidyverse.org/

#plotting dataframes: https://www.geeksforgeeks.org/how-to-plot-all-the-columns-of-a-dataframe-in-r/
#box plots: https://www.sthda.com/english/wiki/ggplot2-box-plot-quick-start-guide-r-software-and-data-visualization
#vectors: https://www.rdocumentation.org/packages/base/versions/3.6.2/topics/c

Func_ReadFiles <- function()
{
  env0NoCulling <- read.csv("Env_0_NoCulling_Averages.csv") %>%   #reads the csv file
    mutate(Env = "DENSE", BFC = 0, FVC = 0, ZC = 0) #adds new headings - binaries idea for culling algorithms given by Michael Scott (thankyou!)
  env1NoCulling <- read.csv("Env_1_NoCulling_Averages.csv") %>%
    mutate(Env = "SPARSE", BFC = 0, FVC = 0, ZC = 0 )
  env2NoCulling <- read.csv("Env_2_NoCulling_Averages.csv") %>% 
    mutate(Env = "DYNAMIC", BFC = 0, FVC = 0, ZC = 0)
  
  env0Frustum <- read.csv("Env_0_FrustumCulling_Averages.csv") %>%
    mutate(Env = "DENSE", BFC = 0, FVC = 1, ZC = 0)
  env1Frustum <- read.csv("Env_1_FrustumCulling_Averages.csv") %>%
    mutate(Env = "SPARSE", BFC = 0, FVC = 1, ZC = 0)
  env2Frustum <- read.csv("Env_2_FrustumCulling_Averages.csv") %>% 
    mutate(Env = "DYNAMIC", BFC = 0, FVC = 1, ZC = 0)
  
  env0Back <- read.csv("Env_0_BackfaceCulling_Averages.csv") %>%
    mutate(Env = "DENSE", BFC = 1, FVC = 0, ZC = 0)
  env1Back <- read.csv("Env_1_BackfaceCulling_Averages.csv") %>%
    mutate(Env = "SPARSE", BFC = 1, FVC = 0, ZC = 0)
  env2Back <- read.csv("Env_2_BackfaceCulling_Averages.csv") %>% 
    mutate(Env = "DYNAMIC", BFC = 1, FVC = 0, ZC = 0)
  
  env0Z <- read.csv("Env_0_ZCulling_Averages.csv") %>%
    mutate(Env = "DENSE", BFC = 0, FVC = 0, ZC = 1)
  env1Z <- read.csv("Env_1_ZCulling_Averages.csv") %>%
    mutate(Env = "SPARSE", BFC = 0, FVC = 0, ZC = 1)
  env2Z <- read.csv("Env_2_ZCulling_Averages.csv") %>%
    mutate(Env = "DYNAMIC", BFC = 0, FVC = 0, ZC = 1)
  
  env0FrustumBack <- read.csv("Env_0_BackfaceAndFrustum_Averages.csv") %>%
    mutate(Env = "DENSE", BFC = 1, FVC = 1, ZC = 0)
  env1FrustumBack <- read.csv("Env_1_BackfaceAndFrustum_Averages.csv") %>%
    mutate(Env = "SPARSE", BFC = 1, FVC = 1, ZC = 0)
  env2FrustumBack <- read.csv("Env_2_BackfaceAndFrustum_Averages.csv") %>%
    mutate(Env = "DYNAMIC", BFC = 1, FVC = 1, ZC = 0)
  
  env0FrustumZ <- read.csv("Env_0_FrustumAndZ_Averages.csv") %>%
    mutate(Env = "DENSE", BFC = 0, FVC = 1, ZC = 1)
  env1FrustumZ <- read.csv("Env_1_FrustumAndZ_Averages.csv") %>%
    mutate(Env = "SPARSE", BFC = 0, FVC = 1, ZC = 1)
  env2FrustumZ <- read.csv("Env_2_FrustumAndZ_Averages.csv") %>%
    mutate(Env = "DYNAMIC", BFC = 0, FVC = 1, ZC = 1)
  
  env0BackZ <- read.csv("Env_0_BackfaceAndZ_Averages.csv") %>%
    mutate(Env = "DENSE", BFC = 1, FVC = 0, ZC = 1)
  env1BackZ <- read.csv("Env_1_BackfaceAndZ_Averages.csv") %>%
    mutate(Env = "SPARSE", BFC = 1, FVC = 0, ZC = 1)
  env2BackZ <- read.csv("Env_2_BackfaceAndZ_Averages.csv") %>%
    mutate(Env = "DYNAMIC", BFC = 1, FVC = 0, ZC = 1)
  
  env0All <- read.csv("Env_0_AllCulling_Averages.csv") %>%
    mutate(Env = "DENSE", BFC = 1, FVC = 1, ZC = 1)
  env1All <- read.csv("Env_1_AllCulling_Averages.csv") %>%
    mutate(Env = "SPARSE", BFC = 1, FVC = 1, ZC = 1)
  env2All <- read.csv("Env_2_AllCulling_Averages.csv") %>%
    mutate(Env = "DYNAMIC", BFC = 1, FVC = 1, ZC = 1)
}


dataFrame <- rbind(env0NoCulling, env1NoCulling, env2NoCulling,
                   env0Frustum, env1Frustum, env2Frustum,
                   env0Back, env1Back, env2Back,
                   env0Z, env1Z, env2Z,
                   env0FrustumBack, env1FrustumBack, env2FrustumBack,
                   env0FrustumZ, env1FrustumZ, env2FrustumZ,
                   env0BackZ, env1BackZ, env2BackZ,
                   env0All, env1All, env2All)

model <- lm(avg..fps ~ avg..polys + avg..models + Env + BFC + FVC + ZC, dataFrame)
summary(model)


Func_Hypothesis1And2 <- function()
{
  dataFrame <- dataFrame %>% mutate(activeAlgorithms = FVC + BFC + ZC)
  cleanDf <- dataFrame[-221,]
  polydf <- ggplot(dataFrame, aes(x = factor(activeAlgorithms), y = avg..polys)) + 
    stat_summary(fun = mean, geom = "point", size = 3) + stat_summary(fun = mean, geom = "line", group = 1) +
    labs(x = "Number of active culling algorithms", y = "average rendered polygons", title = "Model count against number of active culling algorithms")+
    theme_bw()

  GraphPolyAvg0 <- mean(cleanDf$avg..polys[cleanDf$activeAlgorithms == 0])
  GraphPolyAvg1 <- mean(cleanDf$avg..polys[cleanDf$activeAlgorithms == 1])
  GraphPolyAvg2 <- mean(cleanDf$avg..polys[cleanDf$activeAlgorithms == 2])
  GraphPolyAvg3 <- mean(cleanDf$avg..polys[cleanDf$activeAlgorithms == 3])
  
 modeldf <- ggplot(dataFrame, aes(x = factor(activeAlgorithms), y = avg..models)) + 
    stat_summary(fun = mean, geom = "point", size = 3) + stat_summary(fun = mean, geom = "line", group = 1) +
    labs(x = "Number of active culling algorithms", y = "average rendered models", title = "Model count against number of active culling algorithms")+
    theme_bw()
 
 GraphModelAvg0 <- mean(cleanDf$avg..models[cleanDf$activeAlgorithms == 0])
 GraphModelAvg1 <- mean(cleanDf$avg..models[cleanDf$activeAlgorithms == 1])
 GraphModelAvg2 <- mean(cleanDf$avg..models[cleanDf$activeAlgorithms == 2])
 GraphModelAvg3 <- mean(cleanDf$avg..models[cleanDf$activeAlgorithms == 3])
  
  fpsdf <- ggplot(dataFrame, aes(x = factor(activeAlgorithms), y = avg..fps)) + 
    stat_summary(fun = mean, geom = "point", size = 3) + stat_summary(fun = mean, geom = "line", group = 1) +
    labs(x = "Number of active culling algorithms", y = "average frames per second", title = "Model count against number of active culling algorithms")+
    theme_bw()
  
  GraphFPSAvg0 <- mean(cleanDf$avg..fps[cleanDf$activeAlgorithms == 0])
  GraphFPSAvg1 <- mean(cleanDf$avg..fps[cleanDf$activeAlgorithms == 1])
  GraphFPSAvg2 <- mean(cleanDf$avg..fps[cleanDf$activeAlgorithms == 2])
  GraphFPSAvg3 <- mean(cleanDf$avg..fps[cleanDf$activeAlgorithms == 3])
}

Func_Hypothesis3 <- function()
{
  ggplot(dataFrame, aes(x = avg..polys, y = avg..fps, color = Env)) + geom_point() + geom_smooth(method =lm) +  theme_bw() +
    labs(
      title = "Polygon Count against average FPS by environment",
      x = "Polygon Count",
      y = "Average FPS"
    )
}

Func_Hypothesis4 <- function()
{
  ggplot(dataFrame, 
         aes(x = Env, y = avg..fps, fill = factor(BFC))) + geom_boxplot() + labs(
           title = "FPS Based on back face culling",
           x = "Environment",
           y = "Average FPS") +
    theme_minimal()
  
  ggplot(dataFrame, 
         aes(x = Env, y = avg..fps, fill = factor(FVC))) + geom_boxplot() + labs(
           title = "FPS Based on Frustum View Culling",
           x = "Environment",
           y = "Average FPS") +
    theme_minimal()
  
  ggplot(dataFrame, 
         aes(x = Env, y = avg..fps, fill = factor(ZC))) + geom_boxplot() + labs(
           title = "FPS Based on z Culling culling",
           x = "Environment",
           y = "Average FPS") +
    theme_minimal()
}

Func_GetEScore <- function()
{
  defaultValues <- subset(dataFrame, BFC == 0 & FVC == 0 & ZC == 0)
  df2 <- dataFrame[-221,]
  
  #Baseline FPS and Polygon Counts
  avgBaselineFPS <- mean(defaultValues$avg..fps)
  avgBaselinePolys <- mean(defaultValues$avg..polys)
  
  #Average FPS Counts
  
  avgBFCFPS <- mean(dataFrame$avg..fps[dataFrame$BFC == 1 & dataFrame$FVC == 0 & dataFrame$ZC == 0]) #get mean depending on another value
  avgFVCFPS <- mean(df2$avg..fps[dataFrame$BFC == 0 & dataFrame$FVC == 1 & dataFrame$ZC == 0])
  avgZCFPS <- mean(dataFrame$avg..fps[dataFrame$BFC == 0 & dataFrame$FVC == 0 & dataFrame$ZC == 1])
  
  avgBFCFVCFPS <- mean(dataFrame$avg..fps[dataFrame$BFC == 1 & dataFrame$FVC == 1 & dataFrame$ZC == 0])
  avgBFCZCFPS <- mean(dataFrame$avg..fps[dataFrame$BFC == 1 & dataFrame$FVC == 0 & dataFrame$ZC == 1])
  avgFVCZCFPS <- mean(dataFrame$avg..fps[dataFrame$BFC == 0 & dataFrame$FVC == 1 & dataFrame$ZC == 1])
  
  avgCombinedFPS <- mean(dataFrame$avg..fps[dataFrame$BFC == 1 & dataFrame$FVC ==1 & dataFrame$ZC == 1])
  
  #Average Polygon Counts
  
  avgBFCPolys <- mean(dataFrame$avg..polys[dataFrame$BFC == 1 & dataFrame$FVC == 0 & dataFrame$ZC == 0])
  avgFVCPolys <- mean(df2$avg..polys[dataFrame$BFC == 0 & dataFrame$FVC == 1 & dataFrame$ZC == 0])
  avgZCPolys <- mean(dataFrame$avg..polys[dataFrame$BFC == 0 & dataFrame$FVC == 0 & dataFrame$ZC == 1])
  
  avgBFCFVCPolys <- mean(dataFrame$avg..polys[dataFrame$BFC == 1 & dataFrame$FVC == 1 & dataFrame$ZC == 0])
  avgBFCZCPolys <- mean(dataFrame$avg..polys[dataFrame$BFC == 1 & dataFrame$FVC == 0 & dataFrame$ZC == 1])
  avgFVCZCPolys <- mean(dataFrame$avg..polys[dataFrame$BFC == 0 & dataFrame$FVC == 1 & dataFrame$ZC == 1])
  
  avgCombinedPolys <- mean(dataFrame$avg..polys[dataFrame$BFC == 1 & dataFrame$FVC == 1 & dataFrame$ZC == 1])
  
  #EScore Value Calculation
  NoCullingEScore <- ((avgBaselineFPS / avgBaselineFPS) * 100) / ((avgBaselinePolys / avgBaselinePolys) * 100)
  
  BFCEScore <- ((avgBFCFPS / avgBaselineFPS) * 100) / ((avgBFCPolys / avgBaselinePolys) * 100) 
  
  FVCEScore <- ((avgFVCFPS / avgBaselineFPS) * 100) / ((avgFVCPolys / avgBaselinePolys) * 100) 
  
  ZCEScore <- ((avgZCFPS / avgBaselineFPS) * 100) / ((avgZCPolys / avgBaselinePolys) * 100) 
  
  BFCFVCEScore <- ((avgBFCFVCFPS / avgBaselineFPS) * 100) / ((avgBFCFVCPolys / avgBaselinePolys) * 100) 
  
  BFCZCEScore <- ((avgBFCZCFPS / avgBaselineFPS) * 100) / ((avgBFCZCPolys / avgBaselinePolys) * 100) 
  
  FVCZCEScore <- ((avgFVCZCFPS / avgBaselineFPS) * 100) / ((avgFVCZCPolys / avgBaselinePolys) * 100) 
  
  CombinedEScore <- ((avgCombinedFPS / avgBaselineFPS) * 100) / ((avgCombinedPolys / avgBaselinePolys) * 100)
  
  EScoreDataFrame <- data.frame(
    name = c("E.Baseline", "E.BFC", "E.FVC", "E.ZC", "E.BFCFVC", "E.BFCZC", "E.FVCZC", "E.All"),
    value = c(NoCullingEScore, BFCEScore, FVCEScore, ZCEScore, BFCFVCEScore, BFCZCEScore, FVCZCEScore, CombinedEScore)
    )
  
  ggplot(EScoreDataFrame, aes(x = name, y = value)) +
    geom_bar(stat = "identity")
}
Func_ReadFiles()
Func_Hypothesis1And2()
Func_Hypothesis3()
Func_Hypothesis4()
Func_GetEScore()

write.csv(dataFrame, "D:/dataFrame.csv")
write.csv(fpsdf$data, "D:/FpsGraphDataValues.csv")
