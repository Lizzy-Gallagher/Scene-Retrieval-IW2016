/**
 * Created by erb2 on 2/7/17.
 */

import React, { Component } from 'react';
import { Row, Col, Button } from 'react-bootstrap'

import AutoSuggestBox from './AutoSuggestBox'

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
      <input id="query" type="text" className="search-query form-control" placeholder="e.g. living room with sofa"
        value = {this.props.query}
        ref={(input) => {this.textInput = input;}}
        onChange={this.handleChange}
        onKeyPress={this.handleKeyPress}
        style={{height: '44px'}}
      />
    );
  }
}

class QueryBox extends Component {
  constructor() {
    super();
  }

  render() {
    let searchBox = this.props.doEnableAutosuggest ?
                      <AutoSuggestBox query={ this.props.query }
                        handleChange={ this.props.handleChange }
                        onClick={() => this.props.handleClick()} /> :
                      <SearchBox query={ this.props.query }
                        handleChange={ this.props.handleChange }
                        onClick={() => this.props.handleClick()} />

    let inputStyle = {
      margin: '0px -36px'
    }

    return (
      <Row style={{padding: '10px 0px'}}>
        <Col md={8}>
          { searchBox }
        </Col>
        <Col md={1}>
          <Button bsStyle="danger" style={inputStyle} onClick={this.props.onClick} block>
            <i className="fa fa-search fa-2x" aria-hidden="true"></i>
          </Button>
        </Col>
        <Col md={3}/>
      </Row>

  );
  }
}

export default QueryBox;