/**
 * Created by erb2 on 2/7/17.
 */

import React, { Component } from 'react';
// import Autosuggest from 'react-autosuggest'
import { Autocomplete }   from 'material-ui';

let searchImg = require('../images/search.svg');

class SearchBox extends Component {
  constructor(props) {
    super(props);
    this.handleChange = this.handleChange.bind(this);
    this.handleKeyPress = this.handleKeyPress.bind(this);

    this.state = {
      value: '',
      suggestions: []
    };
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
    } else if (target.charCode == 32) {
        alert('A space!')
    }
  }

  onUpdateInput(inputValue) {
  }

  render() {
    let dataSource = ['a', 'b', 'c']
    return (
        <AutoComplete
            dataSource    = {dataSource}
            onUpdateInput = {this.onUpdateInput} />
    );
  }
}

      
      {/*<input id="query" type="text" className="search-query form-control" placeholder="Search"
        value = {this.props.query}
        ref={(input) => {this.textInput = input;}}
        onChange={this.handleChange}
        onKeyPress={this.handleKeyPress}/>*/}

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
            <SearchBox query={ this.props.query }
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
