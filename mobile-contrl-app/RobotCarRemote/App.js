import React from 'react';
import {Animated, Image, PanResponder, StyleSheet, Text, View} from 'react-native';

export default class App extends React.Component {
  constructor(props) {
    super(props);

    this.state = {pan: new Animated.ValueXY(), offsetX: '0', offsetY: '0'};
  }
  render() {
    let {pan} = this.state;

    let [translateX, translateY] = [pan.x, pan.y];
    console.log('X: ' + translateX._value + ' Y: ' + translateY._value);

    let imageStyle = {
      transform: [{translateX}, {translateY}],
      alignItems: 'center',
      justifyContent: 'center',
    };
    return (
        <View style = {styles.container}><View style = {styles.circle}>
        <Animated.View style = {imageStyle} {...this._panResponder.panHandlers}>
        <Image source = {require('./assets/button.png')}>
        </Image>
        </Animated.View>
        </View>
        <Text>X: {String(translateX._value)} Y:{this.state.offsetY}</Text>
        <View></View>
      </View>);
  }

  componentWillMount() {
    this.state.pan.setValue({x: 0, y: 0});

    // let mover =
    //     Animated.event([null, {dx: this.state.pan.x, dy: this.state.pan.y}]);

    this._panResponder = PanResponder.create({
      onMoveShouldSetResponderCapture: () => true,
      onMoveShouldSetPanResponderCapture: () => true,
      onPanResponderGrant: (e, gestureState) => {
        this.state.pan.setOffset(
            {x: this.state.pan.x._value, y: this.state.pan.y._value});
        this.state.pan.setValue({x: 0, y: 0});
      },
      onPanResponderMove: (e, gestureState) => {
        this.setState({offsetX: gestureState.dx, offsetY: gestureState.dy});
        this.state.pan.setValue({x: gestureState.dx, y: gestureState.dy});

        return Animated.event(
            [e, {dx: this.state.pan.x, dy: this.state.pan.y}]);

        // return mover(e, gestureState);
      },
      onPanResponderRelease: (e, {vx, vy}) => {
        this.state.pan.setValue({x: 0, y: 0});
        this.state.pan.flattenOffset();
        this.setState({offsetX: 0, offsetY: 0});
      }
    })
  }
}

const styles = StyleSheet.create({
  container: {
    flex: 1,
    flexDirection: 'column',
    backgroundColor: '#fff',
    alignItems: 'center',
    justifyContent: 'center',
  },
  circle: {
    width: 300,
    height: 300,
    borderRadius: 300 / 2,
    backgroundColor: 'red',
    alignItems: 'center',
    justifyContent: 'center',
  }
});
