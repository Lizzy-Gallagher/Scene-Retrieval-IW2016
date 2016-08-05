# Understanding Scenes

This is a private repository for the scene understanding independent work I performed in Summer 2016 with the Princeton Graphics Group, Professor Thomas Funkhouser, Shuran Song, Manolis Saava, and Angel Chang.


* [Getting Started](#getting-started)
* [Roadmap](#road-map)
* [Utilities](#utilities)

###<a name="getting-started"></a>Getting Started:
- 


###<a name="road-map></a>Roadmap:

- **bin** : scripts for running programs (see below for details)
- **data** : local version contains: Planner5D projects, generated statistics, ML training files
- **gaps** : holds Prof. Funkhouser's C++ libraries and my apps (p5danalyze, relview, scn2rel, scn2meas)
- **old-projects** : workspaces for partial / previous projects (e.g. heatmaps, previous preposition model)
- **rel-proj** : workspace for relationship project (single or many scene, generates ML-ready files)

###<a name="utilities"></a>Utilities:

#### schedule-job (Ionic)
- modify greatly for future use
- for running jobs on Ionic

#### run-relview
- use in a project folder
- run-relview (builds dependencies and runs relview)
- run-relview clean / run-relview clean relationships / run-relview clean measurements

#### run-learning
- use in a project folder
- run-learning (builds dependencies and creates an .arff file)
- run-learning clean / run-learning clean rel/ rr clean meas

#### concat-weka
- run in folder with many .arff files and a header file (produced automatcially by run-learning)
- concat-weka \[alternate-filename\] (concatenates the files into data.arff)

#### filter-weka
- e.g. filter-weka data.arff rug sofa
- for use in sanity checking, creates another .arff file with only the two categories represented

