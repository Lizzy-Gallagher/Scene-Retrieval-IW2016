/**
 * Created by erb2 on 2/7/17.
 */

import React, { Component } from 'react';
import AutoSuggestBox from './AutoSuggestBox'

let searchImg = require('../images/search.svg');

class SearchBox extends Component {
  constructor(props) {
    super(props);
    this.handleChange = this.handleChange.bind(this);
    this.handleKeyPress = this.handleKeyPress.bind(this);
  }

  componentDidMount() {
    this.textInput.focus();
  }

  handleChange() {
    this.props.handleChange(this.textInput.value);
  }

  handleKeyPress(target) {
    if (target.charCode == 13) {
      this.props.onClick()
    }
  }

  render() {
    return (
      <input id="query" type="text" className="search-query form-control" placeholder="Search"
        value = {this.props.query}
        ref={(input) => {this.textInput = input;}}
        onChange={this.handleChange}
        onKeyPress={this.handleKeyPress}/>
    );
  }
}

class SearchButton extends Component {
  render() {
    return (
      <span className="input-group-btn">
        <button id="btnQuery" className="btn btn-danger" type="button" onClick={() => this.props.onClick()}>
          <span className="glyphicon glyphicon-search" />
        </button>
      </span>
    );
  }
}

class QueryBox extends Component {
  constructor() {
    super();
  }

  render() {
    let inputStyle = {
      margin: '10px -27px'
    }
    let searchBox  = this.props.doEnableAutosuggest ? 
                      <AutoSuggestBox query={ this.props.query }
                        handleChange={ this.props.handleChange }
                        onClick={() => this.props.handleClick()} /> :
                      <SearchBox query={ this.props.query }
                        handleChange={ this.props.handleChange }
                        onClick={() => this.props.handleClick()} />

    return (
      <div className="container-fluid">
        <div id="custom-search-input" className="row" style={inputStyle}>
          <div className="input-group col-md-8">
            { searchBox }
            <SearchButton onClick={() => this.props.handleClick()}/>
          </div>
        </div>
      </div>
  );
  }
}

export default QueryBox;
