/**
 * Created by erb2 on 2/9/17.
 */

import React, { Component } from 'react';
import LazyLoad from 'react-lazyload';

let imgStyle = {
  margin: 'auto',
  display: 'block',
  width:'95%'
}

let cardStyle = {
  margin: '3px'
}

class SceneResult extends Component {
  constructor(props) {
    super(props);
  }

  render() {
    let imgURL = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/scenes_rendered/' +
      this.props.sceneId + '/' + this.props.sceneId + '.png';

    return (
      <div className="col-sm-3">
        <div className="card text-center" style={ cardStyle }>
          <img className="card-img-top" src={ imgURL } style={ imgStyle }/>
        </div>
      </div>
    );
  }
}

class LevelResult extends Component {
  constructor(props) {
    super(props);
  }

  render() {
    let imgURL = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/levels_rendered/' +
      this.props.sceneId + '/' + this.props.sceneId + '_' + this.props.level_num + '.png';

    return (
      <div className="col-sm-3">
        <div className="card text-center" style={cardStyle}>
          <div className="card-block">
            <img className="card-img-top" src={ imgURL } style={imgStyle}/>
          </div>
        </div>
      </div>
    );
  }
}

class RoomResult extends Component {
  constructor(props) {
    super(props);
  }

  render() {
    let imgURL = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/rooms_rendered/' +
      this.props.sceneId + '/' + this.props.sceneId + '_' +
      this.props.level_num + '_' +  + this.props.room_num + '.png';

    return (
      <div className="col-sm-3">
        <div className="card text-center" style={cardStyle}>
          <div className="card-block">
            <img className="card-img-top" src={ imgURL } style={imgStyle}/>
          </div>
        </div>
      </div>
    );
  }
}

class ErrorMessage extends Component {
  render() {
    return (
      <div className="alert alert-danger col-sm-2" role="alert" style={{margin: '50px auto', textAlign:'center'}}>
        No results.
      </div>
    )
  }
}

class ResultList extends Component {
  constructor(props) {
    super(props);
  }

  render() {
    let cutoff = 100;
    let resultNodes = [];
    let i = 0;
    if (this.props.showNotFoundError) {
      resultNodes = <ErrorMessage />
    }
    else if (this.props.returnType == 'scene') {

      resultNodes = this.props.sceneData.slice(0,cutoff).map(function (result) {
        return (
          <LazyLoad once offset={100} key={i++} >
            <SceneResult sceneId={result.scene_hash} count={result.value}/>
          </LazyLoad>
        );
      });
    } else if (this.props.returnType == 'level') {
        resultNodes = this.props.levelData.slice(0,cutoff).map(function (result) {
        return (
          <LazyLoad once offset={100} key={i++} >
            <LevelResult sceneId={result.scene_hash} count={result.value}
                       level_num={result.level_num} />
          </LazyLoad>
        );
      });
    } else {
        resultNodes = this.props.roomData.slice(0,cutoff).map(function (result) {
        return (
          <LazyLoad once offset={100} key={i++} >
            <RoomResult sceneId={result.scene_hash} count={result.value}
                      level_num={result.level_num} room_num={result.room_num}/>
          </LazyLoad>
        );
      });
    }

    return (
      <div id="results" className="row">
          { resultNodes }
      </div>
    );
  }
}

export default ResultList;
