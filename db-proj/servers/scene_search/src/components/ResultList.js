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

function createSceneImageCall(hash, objectList) {
    let baseURL = 'http://localhost:2000/highlightscene?'
    baseURL += 'hash=' + hash
    baseURL += '&';
    baseURL += 'objects=' + objectList.join(',')
    return baseURL;
}

function createLevelImageCall(hash, level_num, objectList) {
    let baseURL = 'http://localhost:2000/highlightlevel?'
    baseURL += 'hash=' + hash + '&';
    baseURL += 'level_num=' + level_num + '&';
    baseURL += 'objects=' + objectList.join(',')
    return baseURL;
}

function createRoomImageCall(hash, level_num, room_num, objectList) {
    let baseURL = 'http://localhost:2000/highlightroom?'
    baseURL += 'hash=' + hash + '&';
    baseURL += 'level_num=' + level_num + '&';
    baseURL += 'room_num=' + room_num + '&';
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
    this.refs.regular.style = { display: 'none' }
    this.setState({isLoaded: true})
  }
  
  render() {
    let enhancedImgURL = '';

    if (this.props.doEnableVis)
      enhancedImgURL = createSceneImageCall(this.props.sceneId, this.props.objects);
    

    let regularImgURL = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/scenes_rendered/' +
       this.props.sceneId + '/' + this.props.sceneId + '.png';

    let enhancedStyle = this.state.isLoaded ? imgStyle : { display: 'none' }
    let regularStyle = this.state.isLoaded ? { display: 'none' } : imgStyle

    let enhancedImg = this.props.doEnableVis ?
            <img className="card-img-top" src={ enhancedImgURL } style={ enhancedStyle }
              onLoad={this.handleImageLoaded.bind(this)} ref="enhanced"/> :
              <div></div>

    return (
      <div className="col-sm-3">
        <div className="card text-center" style={ cardStyle }>
          { enhancedImg }
          <img className="card-img-top" src={ regularImgURL } style={ regularStyle } ref="regular"/>
        </div>
      </div>
    );
  }
}

class LevelResult extends Component {
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
    this.refs.regular.style = { display: 'none' }
    this.setState({isLoaded: true})
  }

  render() {
    let enhancedImgURL = '';

    if (this.props.doEnableVis)
      enhancedImgURL = createLevelImageCall(this.props.sceneId, this.props.level_num,
                                         this.props.room_num, this.props.objects);
    let regularImgURL = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/levels_rendered/' +
      this.props.sceneId + '/' + this.props.sceneId + '_' + this.props.level_num + '.png';

    let enhancedStyle = this.state.isLoaded ? imgStyle : { display: 'none' }
    let regularStyle = this.state.isLoaded ? { display: 'none' } : imgStyle

    let enhancedImg = this.props.doEnableVis ?
            <img className="card-img-top" src={ enhancedImgURL } style={ enhancedStyle }
              onLoad={this.handleImageLoaded.bind(this)} ref="enhanced"/> :
              <div></div>
    return (
      <div className="col-sm-3">
        <div className="card text-center" style={cardStyle}>
          <div className="card-block">
            <img className="card-img-top" src={ regularImgURL } style={regularStyle} ref="regular"/>
            { enhancedImg }
          </div>
        </div>
      </div>
    );
  }
}

class RoomResult extends Component {
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
    this.refs.regular.style = { display: 'none' }
    this.setState({isLoaded: true})
  }

  render() {
    let enhancedImgURL = '';

    if (this.props.doEnableVis)
      enhancedImgURL = createRoomImageCall(this.props.sceneId, this.props.level_num,
                                         this.props.room_num, this.props.objects);
    let regularImgURL = 'http://dovahkiin.stanford.edu/fuzzybox/suncg/planner5d/rooms_rendered/' +
      this.props.sceneId + '/' + this.props.sceneId + '_' +
      this.props.level_num + '_' +  + this.props.room_num + '.png';

    let enhancedStyle = this.state.isLoaded ? imgStyle : { display: 'none' }
    let regularStyle = this.state.isLoaded ? { display: 'none' } : imgStyle

    let enhancedImg = this.props.doEnableVis ?
            <img className="card-img-top" src={ enhancedImgURL } style={ enhancedStyle }
              onLoad={this.handleImageLoaded.bind(this)} ref="enhanced"/> :
              <div></div>

    return (
      <div className="col-sm-3">
        <div className="card text-center" style={cardStyle}>
          <div className="card-block">
            { enhancedImg }
            <img className="card-img-top" src={ regularImgURL } style={ regularStyle } ref="regular"/>
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
    let cutoff = 50;
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
            <SceneResult sceneId={result.scene_hash} count={result.value}
                         objects={result.objects} doEnableVis={doEnableVis}/>
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
        let doEnableVis = this.props.doEnableVis;
        resultNodes = this.props.roomData.slice(0,cutoff).map(function (result) {
          
          // bandaid :( -- outdoor spaces
          if (result.room_num == -1)
            return (<div></div>)
        return (
          <LazyLoad once offset={100} key={i++} >
            <RoomResult sceneId={result.scene_hash} count={result.value}
                        level_num={result.level_num} room_num={result.room_num}
                        objects={result.objects} doEnableVis={doEnableVis}/>
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
