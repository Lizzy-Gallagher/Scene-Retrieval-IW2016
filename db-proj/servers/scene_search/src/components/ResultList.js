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

function createImageCall(hash,objectList) {
    let baseURL = 'http://localhost:2000/getimage?'
    baseURL += 'hash=' + hash
    baseURL += '&';
    baseURL += 'objects=' + objectList.join(',')
    return baseURL;
}

class SceneResult extends Component {
  constructor(props) {
    super(props);

    this.state = {
      isLoaded : false
    }
  }

  handleImageLoaded() {
    if (this.state.isLoaded)
      return

    this.refs.enhanced.style = { imgStyle }
    this.refs.regular.style = { display: "none" }
    this.setState({isLoaded: true})
  }
  render() {
    console.log("rendering")
    // let imgURL = this.props.doEnableVis ?
    //   createImageCall(this.props.sceneId, this.props.objects) :
    //   'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/scenes_rendered/' +
    //    this.props.sceneId + '/' + this.props.sceneId + '.png';

    let enhancedImg = createImageCall(this.props.sceneId, this.props.objects);

    let regularImg = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/scenes_rendered/' +
       this.props.sceneId + '/' + this.props.sceneId + '.png';

    let enhancedStyle = this.state.isLoaded ? imgStyle : { display: "none" }
    let regularStyle = this.state.isLoaded ? { display: "none" } : imgStyle

    return (
      <div className="col-sm-3">
        <div className="card text-center" style={ cardStyle }>
          <img className="card-img-top" src={ enhancedImg } style={ enhancedStyle }
                onLoad={this.handleImageLoaded.bind(this)} ref="enhanced"/>
          <img className="card-img-top" src={ regularImg } style={ regularStyle } ref="regular"/>
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
    let cutoff = 5;
    let resultNodes = [];
    let i = 0;
    if (this.props.showNotFoundError) {
      resultNodes = <ErrorMessage />
    }
    else if (this.props.returnType == 'scene') {
      let doEnableVis = this.props.doEnableVis;
      resultNodes = this.props.sceneData.slice(0,cutoff).map(function (result) {
        return (
          <LazyLoad once offset={100} key={i++} >
            <SceneResult sceneId={result.scene_hash} count={result.value} objects={result.objects} doEnableVis={doEnableVis}/>
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
