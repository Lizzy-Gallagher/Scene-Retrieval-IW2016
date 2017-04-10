/**
 * Created by erb2 on 2/7/17.
 */

import React, { Component } from 'react';

import SearchBox from './SearchBox'
import SearchButton from './SearchButton'

class QueryBox extends Component {
  constructor() {
    super();

    this.handleChange = this.handleChange.bind(this);
    this.handleClick  = this.handleClick.bind(this);
  }

  handleChange(text) {

    this.props.handleChange(text);
  }

  handleClick() {
    this.props.handleClick();
  }

  render() {
    let inputStyle = {
      margin: "10px -27px"
    }

    return (
      <div className="container-fluid">
        <div id="custom-search-input" className="row" style={inputStyle}>
          <div className="input-group col-md-8">
            <SearchBox query={ this.props.query } 
                       handleChange={ this.handleChange }
                       onClick={() => this.handleClick()} />
            <SearchButton onClick={() => this.handleClick()}/>
          </div>
        </div>
      </div>
  );
  }
}

QueryBox.defaultProps = {};

export default QueryBox;
