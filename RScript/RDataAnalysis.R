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
  mutate(Env = "DENSE", BFC = 0, FVC = 0, ZC = 0) #adds the environment and algorithm headings
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

ggplot(dataFrame, 
       aes(x = Env, y = avg..fps, fill = Env)) +
  geom_boxplot() +
  labs(title = "FPS Across Environments", x = "Environment", y = "Average FPS") +
  theme_minimal()

ggplot(dataFrame, 
       aes(x = Env, y = avg..fps, fill = factor(BFC))) +
  geom_boxplot() +
  labs(title = "FPS Based on back face culling", x = "Environment", y = "Average FPS") +
  theme_minimal()

ggplot(dataFrame, 
       aes(x = Env, y = avg..fps, fill = factor(FVC))) +
  geom_boxplot() +
  labs(title = "FPS Based on Frustum View Culling", x = "Environment", y = "Average FPS") +
  theme_minimal()

ggplot(dataFrame, 
       aes(x = Env, y = avg..fps, fill = factor(ZC))) +
  geom_boxplot() +
  labs(title = "FPS Based on z Culling culling", x = "Environment", y = "Average FPS") +
  theme_minimal()

ggplot(dataFrame, 
       aes(x = avg..models, y = avg..fps, color = Env)) +
  geom_point(alpha = 1.0) +
  geom_smooth(method = "1m", se = FALSE) +
  facet_grid(BFC ~ FVC) +
  labs(title = "FPS Trends based on Back face culling and Frustum Culling combined", x =  "Average Models", y = "Average FPS") +
  theme_minimal()

