/**
 * Created by erb2 on 2/9/17.
 */

import React, { Component } from 'react';
import { Line } from  'rc-progress'

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
            {/*<p className="card-text">Scene: { this.props.sceneId }</p>*/}
            {/*<p className="card-text">Score: { this.props.count }</p>*/}
            {/*<Line percent={ this.props.partialSceneData[0].results[this.props.i].value } strokeWidth="4" strokeColor="#D3D3D3"/>*/}
            {/*<Line percent={ this.props.partialSceneData[1].results[this.props.i].value} strokeWidth="4" strokeColor="#D3D3D3"/>*/}
            {/*<Line percent={ this.props.count } strokeWidth="4" strokeColor="#D3D3D3" />*/}
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
            {/*<p className="card-text">Scene: { this.props.sceneId }</p>*/}
            {/*<p className="card-text">Level: { this.props.level_num }</p>*/}
            {/*<p className="card-text">Occurences: { this.props.count }</p>*/}
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
            {/*<p className="card-text">Scene: { this.props.sceneId }</p>*/}
            {/*<p className="card-text">Level: { this.props.level_num }</p>*/}
            {/*<p className="card-text">Room: { this.props.room_num }</p>*/}
            {/*<p className="card-text">Occurences: { this.props.count }</p>*/}
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
    let resultNodes = [];
    let self = this;
    
    let i = -1;
    if (this.props.showNotFoundError) {
      resultNodes = <ErrorMessage />
    }
    else if (this.props.returnType == 'scene') {

      resultNodes = this.props.sceneData.slice(0,24).map(function (result) {
        return (
          <SceneResult sceneId={result.scene_hash} count={result.value}
                       key={i} i={++i} />
        );
      });
    } else if (this.props.returnType == 'level') {
        resultNodes = this.props.levelData.slice(0,24).map(function (result) {
        return (
          <LevelResult sceneId={result.scene_hash} count={result.value}
                       level_num={result.level_num} key={++i} />
        );
      });
    } else {
        resultNodes = this.props.roomData.slice(0,24).map(function (result) {
        return (
          <RoomResult sceneId={result.scene_hash} count={result.value}
                      level_num={result.level_num} room_num={result.room_num}
                      key={++i} />
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
