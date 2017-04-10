/**
 * Created by erb2 on 2/8/17.
 */

import React, { Component } from 'react';

class SearchBox extends Component {
  constructor(props) {
    super(props);
    this.handleChange = this.handleChange.bind(this);
    this.handleKeyPress = this.handleKeyPress.bind(this);
  }

  focus() {
    this.textInput.focus();
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
      <input
        id="query"
        type="text"
        className="search-query form-control"
        placeholder="Search"
        value = {this.props.query}
        ref={(input) => {this.textInput = input;}}
        onChange={this.handleChange}
        onKeyPress={this.handleKeyPress}/>
    );
  }
}

SearchBox.defaultProps = {};

export default SearchBox;
