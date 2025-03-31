library(dplyr)
library(ggplot2)
library(scales)

#Code for the data is a mix of basic RStudio code, and libraries. The library code is explained below.

# "%>%" is the pipe operator. Allows you to do stuff to the variable assigned on that line, makes code neater
# https://magrittr.tidyverse.org/reference/pipe.html

# "mutate()" is a dplyr function, that adds new variables to existing data
# https://dplyr.tidyverse.org/

#for this data, add the culling algorithms as a binary, and set to 0 or 1 for active or not.
# use linear regression - lm(avg..fps ~ avg..models + Env1 + Env2 + Env3 + Alg1 + Alg2 + Alg3, dataFrame)
# if this doens't work, ANOVA works fine

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

dataFrame <- rbind(env0NoCulling, env1NoCulling, env1NoCulling,
                   env0Frustum, env1Frustum, env2Frustum,
                   env0Back, env1Back, env2Back,
                   env0Z, env1Z, env2Z,
                   env0FrustumBack, env1FrustumBack, env2FrustumBack,
                   env0FrustumZ, env1FrustumZ, env2FrustumZ,
                   env0BackZ, env1BackZ, env2BackZ,
                   env0All, env1All, env2All)

model <- lm(avg..fps ~ avg..polys + avg..models + Env + BFC + FVC + ZC, dataFrame)
summary(model)

#plotting dataframes: https://www.geeksforgeeks.org/how-to-plot-all-the-columns-of-a-dataframe-in-r/

#facet grid code found here: https://ggplot2.tidyverse.org/reference/facet_grid.html

ggplot(dataFrame, 
       aes(x = avg..models / 1000, y = avg..fps, color = Env)) +
  geom_point(alpha = 0.6) +
  geom_smooth(method = "1m", confidenceInterval = FALSE) +
  facet_grid(FVC ~ ZC + BFC) +
  labs(title = "FPS Trends based on All Three Culling Algorithms Combined", x ="Average Models (Thousands)", y = "Average FPS") +
  theme_bw()

ggplot(dataFrame, 
       aes(x = avg..polys / 1000000, y = avg..fps, color = Env)) +
  geom_point(alpha = 0.6) +
  geom_smooth(method = "1m", confidenceInterval = FALSE) +
  facet_grid(FVC ~ ZC + BFC) +
  labs(title = "FPS Trends based on All Three Culling Algorithms Combined", x ="Average Polygons (Millions)", y = "Average FPS") +
  theme_bw()

