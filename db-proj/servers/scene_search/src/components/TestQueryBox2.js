/**
 * Created by erb2 on 2/7/17.
 */

import React, { Component } from 'react';
import Autosuggest from 'react-autosuggest'

import 'whatwg-fetch'

var shuffle = require('shuffle-array')
let searchImg = require('../images/search.svg');

// Use your imagination to render suggestions.
const renderSuggestion = suggestion => (
  <div>
    {suggestion}
  </div>
);

function renderSectionTitle(section) {
  return (
    <strong>{section.title}</strong>
  );
}

function getSectionSuggestions(section) {
  return section.suggestions;
}

class SearchBox extends Component {
  constructor(props) {
    super(props);
    this.handleChange = this.handleChange.bind(this);
    this.handleKeyPress = this.handleKeyPress.bind(this);

    this.onSuggestionSelected = this.onSuggestionSelected.bind(this);
    this.waitOnSuggestions = this.waitOnSuggestions.bind(this);
    this.renderInputComponent = this.renderInputComponent.bind(this);

    this.state = {
      value: '',
      suggestions: []
    };
  }

  renderInputComponent = inputProps => (
    <input 
      id="query" type="text" className="search-query form-control" placeholder="Search"
      onKeyPress={this.handleKeyPress}
      autoFocus
      {...inputProps} 
    />
);

  waitOnSuggestions(query) {
    var self = this;
    
    let queryParams = query.split(' ').join('+');
    if (queryParams[queryParams.length-1] === '+')
      queryParams = queryParams.slice(0, queryParams.length-1)
    let url = 'http://localhost:4000/autocomplete?query=' + queryParams;
    console.log(url)
    
    fetch(url).then(function(response){
      return response.json();
    }).then(function(json){
      console.log(json)

      Object.keys(json.suggestions).forEach(function(key) {
        console.log(key);
        console.log(key)
        if (json.suggestions[key].suggestions.length > 3)
          json.suggestions[key].suggestions = shuffle(json.suggestions[key].suggestions, {'copy': true}).slice(0, 3);
      });

      self.setState({
        suggestions: json.suggestions
      });
    })
  }

  handleChange() {
    this.props.handleChange(this.textInput.value);
  }

  onChange = (event, { newValue }) => {
    this.props.handleChange(newValue);
  };

  // Autosuggest will call this function every time you need to update suggestions.
  onSuggestionsFetchRequested = ({ value }) => {
    console.log('onSuggestionsFetchRequested')
    this.waitOnSuggestions(value)
  };

  // Autosuggest will call this function every time you need to clear suggestions.
  onSuggestionsClearRequested = () => {
    this.setState({
      suggestions: []
    });
  };

  shouldRenderSuggestions(value) {
    return value[value.length-1] === ' '
  }

  handleKeyPress(target) {
    if (target.charCode == 13) {
      this.props.onClick()
    }
  }

  onSuggestionSelected(event, { suggestion, suggestionValue, suggestionIndex, sectionIndex, method }) {
    this.props.handleChange(this.props.query + suggestion)
  }

  render() {
    const { value, suggestions } = this.state;

    // Autosuggest will pass through all these props to the input element.
    const inputProps = {
      placeholder: 'Search',
      value: this.props.query,
      onChange: this.onChange
    };

    return (
      <Autosuggest
        suggestions={suggestions}
        onSuggestionsFetchRequested={this.onSuggestionsFetchRequested}
        onSuggestionsClearRequested={this.onSuggestionsClearRequested}
        getSuggestionValue={suggestion => suggestion}
        getSectionSuggestions={getSectionSuggestions}
        renderSuggestion={renderSuggestion}
        renderSectionTitle={renderSectionTitle}
        shouldRenderSuggestions={this.shouldRenderSuggestions}
        onSuggestionSelected={this.onSuggestionSelected}
        multiSection={true}
        inputProps={inputProps}
        renderInputComponent={this.renderInputComponent}
      />
    );
  }
}


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
