library(dplyr)

#Code for the data is a mix of basic RStudio code, and libraries. The library code is explained below.

# "%>%" is the pipe operator. Allows you to do stuff to the variable assigned on that line, makes code neater
# https://magrittr.tidyverse.org/reference/pipe.html

# "mutate()" is a dplyr function, that adds new variables to existing data
# https://dplyr.tidyverse.org/

env0NoCulling <- read.csv("Env_0_NoCulling_Averages.csv") %>%   #reads the csv file
  mutate(Env = "DENSE", Alg = "None") #adds the environment and algorithm headings
env1NoCulling <- read.csv("Env_1_NoCulling_Averages.csv") %>%
  mutate(Env = "SPARSE", Alg = "None")
env2NoCulling <- read.csv("Env_2_NoCulling_Averages.csv") %>% 
  mutate(Env = "DYNAMIC", Alg = "None")

env0Frustum <- read.csv("Env_0_FrustumCulling_Averages.csv") %>%
  mutate(Env = "DENSE", Alg = "FrustumCulling")
env1Frustum <- read.csv("Env_1_FrustumCulling_Averages.csv") %>%
  mutate(Env = "SPARSE", Alg = "FrustumCulling")
env2Frustum <- read.csv("Env_2_FrustumCulling_Averages.csv") %>% 
  mutate(Env = "DYNAMIC", Alg = "FrustumCulling")

env0Back <- read.csv("Env_0_BackfaceCulling_Averages.csv") %>%
  mutate(Env = "DENSE", Alg = "BackCulling")
env1Back <- read.csv("Env_1_BackfaceCulling_Averages.csv") %>%
  mutate(Env = "SPARSE", Alg = "BackCulling")
env2Back <- read.csv("Env_2_BackfaceCulling_Averages.csv") %>% 
  mutate(Env = "DYNAMIC", Alg = "BackCulling")

env0Z <- read.csv("Env_0_ZCulling_Averages.csv") %>%
  mutate(Env = "DENSE", Alg = "Z-Culling")
env1Z <- read.csv("Env_1_ZCulling_Averages.csv") %>%
  mutate(Env = "SPARSE", Alg = "Z-Culling")
env2Z <- read.csv("Env_2_ZCulling_Averages.csv") %>%
  mutate(Env = "DYNAMIC", Alg = "Z-Culling")

env0FrustumBack <- read.csv("Env_0_BackfaceAndFrustum_Averages.csv") %>%
  mutate(Env = "DENSE", Alg = "FrutumBackCulling")
env1FrustumBack <- read.csv("Env_1_BackfaceAndFrustum_Averages.csv") %>%
  mutate(Env = "SPARSE", Alg = "FrutumBackCulling")
env2FrustumBack <- read.csv("Env_2_BackfaceAndFrustum_Averages.csv") %>%
  mutate(Env = "DYNAMIC", Alg = "FrutumBackCulling")

env0FrustumZ <- read.csv("Env_0_FrustumAndZ_Averages.csv") %>%
  mutate(Env = "DENSE", Alg = "FrustumZCulling")
env1FrustumZ <- read.csv("Env_1_FrustumAndZ_Averages.csv") %>%
  mutate(Env = "SPARSE", Alg = "FrustumZCulling")
env2FrustumZ <- read.csv("Env_2_FrustumAndZ_Averages.csv") %>%
  mutate(Env = "DYNAMIC", Alg = "FrustumZCulling")

env0BackZ <- read.csv("Env_0_BackfaceAndZ_Averages.csv") %>%
  mutate(Env = "DENSE", Alg = "BackZCulling")
env1BackZ <- read.csv("Env_1_BackfaceAndZ_Averages.csv") %>%
  mutate(Env = "SPARSE", Alg = "BackZCulling")
env2BackZ <- read.csv("Env_2_BackfaceAndZ_Averages.csv") %>%
  mutate(Env = "DYNAMIC", Alg = "BackZCulling")

env0All <- read.csv("Env_0_AllCulling_Averages.csv") %>%
  mutate(Env = "DENSE", Alg = "All")
env1All <- read.csv("Env_1_AllCulling_Averages.csv") %>%
  mutate(Env = "SPARSE", Alg = "All")
env2All <- read.csv("Env_2_AllCulling_Averages.csv") %>%
  mutate(Env = "DYNAMIC", Alg = "All")

dataFrame <- rbind(env0NoCulling, env1NoCulling, env1NoCulling,
                   env0Frustum, env1Frustum, env2Frustum,
                   env0Back, env1Back, env2Back,
                   env0Z, env1Z, env2Z,
                   env0FrustumBack, env1FrustumBack, env2FrustumBack,
                   env0FrustumZ, env1FrustumZ, env2FrustumZ,
                   env0BackZ, env1BackZ, env2BackZ,
                   env0All, env1All, env2All)

