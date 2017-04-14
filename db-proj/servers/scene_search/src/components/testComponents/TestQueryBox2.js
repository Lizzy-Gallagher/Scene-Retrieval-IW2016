/**
 * Created by erb2 on 2/7/17.
 */

import React, { Component } from 'react';
import Autosuggest from 'react-autosuggest'

import 'whatwg-fetch'
import AutoSuggestBox from './AutoSuggestBox'

var shuffle = require('shuffle-array')
let searchImg = require('../images/search.svg');




class SearchButton extends Component {
  render() {

    return (
      <span className="input-group-btn">
        <button id="btnQuery" className="btn btn-danger" type="button" onClick={() => this.props.onClick()}>
          <img src={ searchImg } />
          <span className="glyphicon glyphicon-search" />
        </button>
      </span>
    );
  }
}

class TestQueryBox extends Component {
  constructor() {
    super();
  }

  render() {
    let inputStyle = {
      margin: '10px -27px'
    }

    return (
      <div className="container-fluid">
        <div id="custom-search-input" className="row" style={inputStyle}>
          <div className="input-group col-md-8">
            <AutoSuggestBox query={ this.props.query }
                       handleChange={ this.props.handleChange }
                       onClick={() => this.props.handleClick()} />
            <SearchButton onClick={() => this.props.handleClick()}/>
          </div>
        </div>
      </div>
  );
  }
}

export default TestQueryBox;
