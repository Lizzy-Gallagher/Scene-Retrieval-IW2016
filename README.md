# Understanding Scenes

This is a private repository for the scene understanding independent work I performed in Summer 2016 with the Princeton Graphics Group, Professor Thomas Funkhouser, Shuran Song, Manolis Saava, and Angel Chang.


* [Getting Started](#getting-started)
* [Roadmap](#road-map)
* [Utilities](#utilities)

###<a name="getting-started"></a>Getting Started:
*Note:* This section is a work in progress. Check back for changes

1. Clone this repo 
```sh
git clone https://github.com/LizzyBradley/Understanding-Scenes.git
```

2. The project in its current state relies on a very specific hierarchy for Planner5d files. Therefore, you'll need to symlink the following folders to `data/`. Run the following from this repo's root.

```sh
ln -s path/to/projects data/projects
ln -s path/to/objects data/objects
ln -s path/to/roomfiles data/roomfiles
ln -s path/to/textures data/textures
```

Additionally, initiliaze the following directories.

```sh
mkdir data/measurements
mkdir data/relview
mkdir data/weka
```

3. Due to GAPS' size, only the application directories are included. Copy those folders into your own implementation of GAPs and compile them. As long as the newly created binaries are in your PATH they will work.

4. Add `bin/` to your path. Each script explains how to use it and provides examples. `run-relview` visualize relationships for a given project. `run-learning` collects information about object relationships for use in machine learning. To actually perform learning, run `concat-weka` from `data/weka` after using `run-learning` on a number of projects. The resulting data.arff file may then be used in Weka.

###<a name="road-map"></a>Roadmap:

- **bin** : scripts for running programs (see below for details)
- **data** : local version contains: Planner5D projects, generated statistics, ML training files
- **gaps** : holds Prof. Funkhouser's C++ libraries and my apps (p5danalyze, relview, scn2rel, scn2meas)
- **old-projects** : workspaces for partial / previous projects (e.g. heatmaps, previous preposition model)
- **rel-proj** : workspace for relationship project (single or many scene, generates ML-ready files)

###<a name="utilities"></a>Utilities:
The top-level folder bin/ contains all the utility scripts necessary to work with this repository.

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

